/* #region LIBRARIES */
#include "esp_cdc.h"         //* Library that Repackages Original ESP32-S3-MINI-1 Firmware Enabling Original CDC Bridge and CMSIS-DAP Behaviour

#include <WiFi.h>               //* WiFi Library For Hosting AP
#include <WebSocketsServer.h>   //* WebSockets Library for Hosting WebSockets for Asynchronous Communication
#include <ArduinoJson.h>        //* JSON Library for Creating Json Formatted Variables, For Sending to the Client Over WebSockets
#include <PacketSerial.h>
#include <queue>

#include <functional>           //* Tools for Function Objecst, Lambdas and std::funciton Wrappers
#include <unordered_map>        //* Hash Table-Based Container Library
#include <string>

#include "webpage.h"            //* Header File with HTML, CSS, JS for the Webpage

/* #endregion */

/* #region WIFI */

//* Server Ports
WiFiServer server(80);          // Initialise a server on port 80, port 80 is standard for HTTP traffic, port 443 is for HTTPS
WebSocketsServer wss(81);       // Initialise a WebSocket Server on port 81, 81 is the standard for WS

//*IP, Gateway, and Subnet Config
IPAddress local_IP(192, 168, 1, 1);  // AP's static IP
IPAddress gateway(192, 168, 1, 1);   // Gateway
IPAddress subnet(255, 255, 255, 0);  // Subnet Mask

// AP Credentials
const char ssid[] = "Y13ARDUINO";
const char pass[] = "123456789";

/* #endregion*/

/* #region STRUCTS */

//* Local Data Storage
struct LocalData{
  bool enable = false;
  int mode = 0;
  bool obstacle = false;
  int distance = 0;
  float travelled = 0;
  int speed = 0;
  float BuggySpeed = 0;
  int TagID = 0;
  long loop;
};

//* Packet for Sending Command and value Pairs
template<typename T>
struct DataPacket {
  char command[4];
  T value;

  DataPacket(const char cmd[4], T val) : value(val) {
    memcpy(command, cmd, 4);  // Copy the command into the structure
  }
};

/* #endregion*/

/* #region VARIABLES*/

LocalData Data; // Current Data
LocalData PrevData; // Previoud Data, to compare if it changed

PacketSerial serialPacket;

// Message Interval for executing every X ms
const uint32_t message_interval = 50;

std::queue<unsigned long> timeAverage; // For the average loop time
int sum = 0;
int quantity = 50; // Number of samples to average

unsigned long start;
unsigned long ending = 0;
unsigned long loopPrev = 0;

int ESPLoop = 0;
int prevESPLoop = 0;
int PeakLoop = 0;
int prevPeakLoop = 0;

float x = 0, y = 0;
float pastX = 0, pastY = 0;

/* #endregion */

/* #region FUNCTION PROTOTYPES*/

//* Function Prototypes
void webSocketEvent(uint8_t, WStype_t, uint8_t*, size_t);
void SendData();
void HTTP();

void onPacketReceived(const uint8_t* buffer, size_t size);
void CheckAndSend();

void sortPacket();

template <typename T>
void SendUpdate(const char command[4], T value);

/* #endregion */

void setup() {
  // Initialize CDC Bridge and CMSIS-DAP
  esp32_cdc();
  Serial1.begin(115200);
  serialPacket.setStream(&Serial1);
  serialPacket.setPacketHandler(&onPacketReceived);

  // WIFI CONFIG
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, pass);

  // WEB & WS START
  server.begin();
  wss.begin();
  wss.onEvent(webSocketEvent); // Registers the Callback Function
}

void loop() {
  start = micros(); // Get the current time in milliseconds

  serialPacket.update();
  HTTP();  // Sends Initial HTTP Handshake
  wss.loop(); // Processes incoming Events, callback function has to be registered, basically passes the event to the callback function
  CheckAndSend();

  ending = micros(); // Get the current time in milliseconds

  timeAverage.push(ending - start);
  sum += ending - start;

  if (timeAverage.size() > quantity) {
    sum -= timeAverage.front();
    timeAverage.pop();
    ESPLoop = (ending - start);
  }

  if (ESPLoop > PeakLoop) {
    PeakLoop = ESPLoop;
    }
  
  static unsigned long lastExecutionTime = 0;
  if (millis() - lastExecutionTime >= 1000) {
    lastExecutionTime = millis();
    PeakLoop = 0;
  }
}

//* EVENT HANDLER FOR PACKETSERIAL
void onPacketReceived(const uint8_t* buffer, size_t size) {

  char command[4] = {};
  memcpy(command, buffer, 4);

  static const std::unordered_map<std::string, std::function<void(const uint8_t*)>> commandMap = {
    {"ENA", [](const uint8_t* buf) { Data.enable = reinterpret_cast<const DataPacket<bool>*>(buf)->value; }},
    {"MOD", [](const uint8_t* buf) { Data.mode = reinterpret_cast<const DataPacket<int>*>(buf)->value; }},
    {"OBS", [](const uint8_t* buf) { Data.obstacle = reinterpret_cast<const DataPacket<bool>*>(buf)->value; }},
    {"DIS", [](const uint8_t* buf) { Data.distance = reinterpret_cast<const DataPacket<int>*>(buf)->value;  }},
    {"BSP", [](const uint8_t* buf) { Data.BuggySpeed = reinterpret_cast<const DataPacket<float>*>(buf)->value; }},
    {"TAG", [](const uint8_t* buf) { Data.TagID = reinterpret_cast<const DataPacket<int>*>(buf)->value; }},
    {"TRV", [](const uint8_t* buf) { Data.travelled = reinterpret_cast<const DataPacket<float>*>(buf)->value; }},
    {"LOP", [](const uint8_t* buf) { Data.loop = reinterpret_cast<const DataPacket<long>*>(buf)->value; }},
    {"SPD", [](const uint8_t* buf) { Data.speed = reinterpret_cast<const DataPacket<int>*>(buf)->value; }},
    {"POX", [](const uint8_t* buf) { x = reinterpret_cast<const DataPacket<float>*>(buf)->value; }},
    {"POY", [](const uint8_t* buf) { y = reinterpret_cast<const DataPacket<float>*>(buf)->value; }},
    {"QRY", [](const uint8_t* buf) { Serial.println("QRY RECEIVED"); }}
  };

  auto it = commandMap.find(command);
  auto assignment = it->second;

  assignment(buffer);

  if (command != "POX") {
    SendData();
  }
}

//* WEBSOCKET EVENT FUNCTION, IT IS USED BY THE SERVER TO HANDLE ANY KIND OF EVENT, IT HAS TO BE REGISTERED IN SETUP AS THE SERVERS CALLBACK FUNCTION
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  if (type == WStype_TEXT) {  // IF TYPE IS TEXT

    String message = (char*)payload;  // PUT THE PAYLOAD INTO A STRING

    // CHECK WHAT MESSAGE IS BEING RECEIVED
    if (message == "start") {
      Data.enable = true;

    } else if (message == "stop") {
      Data.enable = false;
      // Data.BuggySpeed = 0.0;
      
    } else if (message == "changeMode") {
      Data.mode++;
      if (Data.mode > 1) {
        Data.mode = 0;
      }

    /*

    The messages for numbered values, or non boolean values are sent with a header, like "speed:123",
    where speed tells me what value the number is refering to,
    and the : is used as a separator so I can filter out the header.

    The number is then just converted from string to either an Int or Float type.

    */

    } else if (message.indexOf("speedValue") != -1) {
      int separatorIndex = message.indexOf(":");
      Data.speed = message.substring(separatorIndex + 1).toFloat();
    }

  } else if (type == WStype_CONNECTED) {
    SendData();
  }
  // SendData();
}

//* BROADCASTS VARIABLE VALUES TO ALL CLIENTS ON THE SERVER TO UPDATE THE CLIENTS UI
void SendData() {

  // ADD ALL THE VALUES TO A JSON DOC FOR EASY SEPARATION BETWEEN VARIABLES AND ALLOWING ME TO SEND ALL VALUES AT ONCE
  JsonDocument doc;
  doc["enable"] = Data.enable;
  doc["speed"] = Data.speed;
  doc["mode"] = Data.mode;
  doc["obstacle"] = Data.obstacle;
  doc["buggyspeed"] = Data.BuggySpeed;
  doc["distance"] = Data.distance;
  doc["travelled"] = Data.travelled;
  doc["TagID"] = Data.TagID;
  doc["renesas"] = Data.loop;
  doc["esp"] = ESPLoop;
  doc["peak"] = PeakLoop;
  doc["x"] = x;
  doc["y"] = y;

  // JSON SERIALISATION, CONVERTING THE DOC TO A SINGLE STRING AND PUTTING IT INTO THE VARIABLE "output"
  String output;
  if (serializeJson(doc, output) == 0) {
    return;  // Exit the function if serialization fails
  }

  // broadcasts "output" to all Clients
  wss.broadcastTXT(output);
}

//* INITIAL HTTP HANDSHAKE
void HTTP() {

  // CREATES CLIENT IF THERE IS ONE AVAILABLE
  WiFiClient client = server.available();
  if (client) {
    String request = "";
    while (client.available()) {
      char c = client.read();
      request += c; // reads the request from client
    }

    // HTML REQUEST
    if (request.indexOf("GET / ") >= 0) {

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();

      client.println(html);
    } 
    
    // CSS REQUEST
    else if (request.indexOf("GET /style.css") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/css");
      client.println();

      client.println(css);
    } 
    
    // JS REQUEST
    else if (request.indexOf("GET /script.js") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/javascript");
      client.println();

      client.println(js);
    }
    client.stop();
  }
}

//* CHECK FOR CHANGES IN VALUES AND IF THEY WERE THEN SEND THE UPDATE TO RENESAS
void CheckAndSend() {
  bool changed = false;

  if (Data.enable != PrevData.enable) {
    SendUpdate("ENA", Data.enable);
    changed = true;
  }

  if (Data.mode != PrevData.mode) {
    SendUpdate("MOD", Data.mode);
    changed = true;
  }

  if (Data.speed != PrevData.speed) {
    SendUpdate("SPD", Data.speed);
    changed = true;
  }

  // if (Data.distance != PrevData.distance) {
  //   SendUpdate("DIS", Data.distance);
  //   changed = true;
  // }

  if (ESPLoop != prevESPLoop) {
    prevESPLoop = ESPLoop;
    changed = true;
  }

  if (PeakLoop != prevPeakLoop) {
    prevPeakLoop = PeakLoop;
    changed = true;
  }

  if (x != pastX || y != pastY) {
    pastX = x;
    pastY = y;
    changed = true;
  }

  if (changed) {
    PrevData = Data;
    SendData();
  }
}

//* SEND UPDATES TO RENESAS
template <typename T>
void SendUpdate(const char command[4], T value) {
  DataPacket<T> packet(command, value);
  serialPacket.send((uint8_t*)&packet, sizeof(DataPacket<T>));
}