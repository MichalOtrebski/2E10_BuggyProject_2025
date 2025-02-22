#include <WiFiS3.h>
#include <NewPing.h>      //* Library for the HC-SR04 US Rangefinder, it does the rangefinding
#include <PID_v1.h>       //* PID Library for Computing PID Output
#include <string>
#include <math.h>

char ssid[] = "Y13ARDUINO";
char pass[] = "123456789";

WiFiServer server(5200);

WiFiClient client;

//* IR sensor Pins
const int IR_LEFT = 18; // 18
const int IR_RIGHT = 9; // 9

//* US Rangefinder Pin
const int TRIG = 8; // 8
const int ECHO = 17; // 17

//* Left Motor Pins
const int LEFT1 = 14; // 14
const int LEFT2 = 15; // 15
const int LEFT_HALL = 2; // 2

//* Right Motor Pins
const int RIGHT1 = 16; // 16
const int RIGHT2 = 7; // 7
const int RIGHT_HALL = 3; // 3

//* Motor Control Pins
const int L_MOT = 5;  // PWM pins 5
const int R_MOT = 6;  // PWM pins 6

//* Local Data Storage
struct LocalData{
  bool enable = false;
  int mode = 0;
  bool obstacle = false;
  long distance = 0;
  volatile double travelled = 0;
  int speed = 0;
  int BuggySpeed = 0;
};

struct EncoderData {
  volatile long pulse;
  volatile float distance;
  volatile float speed;
  volatile unsigned long last;
};

LocalData Data; //* Current Data
LocalData PrevData; //* Previous Data to Compare

EncoderData leftHall = {0, 0.0, 0.0, 0};
EncoderData rightHall = {0, 0.0, 0.0, 0};

//Pulses Per Revolution
const double PPR = 4.0;
const float CIRCUM = 6.5 * M_PI; // in cm
const int timeout = 200000;

// IR SENSOR OUTPUT
bool L_IR_O; 
bool R_IR_O;

bool black = true; //! true == on black surface || false == on light surface

// Vars for Computing Loop Execution Time
unsigned long starting;
unsigned long ending;

// Message Interval for executing every X ms
const int message_interval = 50;

// TIMING VARIABLES
unsigned long now;
unsigned long prev;
unsigned long prev_send;

String data;

//*HC-SR04 Sonar
NewPing sonar(TRIG, ECHO, 50);

// TURNING PID VARIABLES
double turningInput; 
double turningSetpoint = 0;
double turningOutput;

double turningKp = 45;
double turningKi = 0.025; 
double turningKd = 2.5;

// STRAIGHT PID VARIABLES
double straightInput = 0; 
double straightSetpoint = 0; 
double straightOutput;

double straightKp = 45;
double straightKi = 0.025; 
double straightKd = 2.5; 

PID turningPID(&turningInput, &turningOutput, &turningSetpoint, turningKp, turningKi, turningKd, DIRECT);
PID straightPID(&straightInput, &straightOutput, &straightSetpoint, straightKp, straightKi, straightKd, DIRECT);

double LeftSpeed;
double RightSpeed;

int baseTurningSpeed = 100;
int scaledTurnSpeed;
int TurningSpeed;

bool set = false;

void setup() {
  Serial.begin(115200);

  WiFi.beginAP(ssid, pass);
  WiFi.config(IPAddress(192, 168, 1, 1));
  
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi Ready");

  server.begin();

  turningPID.SetMode(AUTOMATIC);
  straightPID.SetMode(AUTOMATIC);

  PinInitialise();
}

void loop() {
  starting = micros();
  now = millis();

  Data.BuggySpeed = (leftHall.speed + rightHall.speed) / 2.0;
  Data.travelled = (leftHall.distance + rightHall.distance) / 2.0;

  //Serial.println(Data.travelled);

  CheckAndSend();

  if (now - prev_send >= 200) {
    ServerExchange();
    //Serial.println("sending...");
    prev_send = now;
  }

  checkTimeout();

  // ON/OFF
  if (Data.enable) {

    ReadIR();
    obstacle();

    turningInput = -abs(L_IR_O - R_IR_O);

    turningPID.Compute();

    
    scaledTurnSpeed = baseTurningSpeed + Data.speed*(0.3);
    TurningSpeed = constrain(turningOutput, 0, 255);

    LeftSpeed = scaledTurnSpeed - TurningSpeed;

    // Serial.println(turningInput);
    // Serial.println(turningOutput);

    //! NOT FINAL

    // DEFAULT MODE
    if (Data.mode == 0) {

      //! CODE TO BE REFINED, NOT FINAL, WITH MINOR BUGS
      if (Data.obstacle) {
        stop();
      } else if (L_IR_O && R_IR_O) {  // IF BOTH PINS ARE ON, MEANING LINE IS IN THE MIDDLE
        forward(Data.speed);
      } else if (!L_IR_O && R_IR_O) { // IF LEFT PIN TURNS OFF AND RIGHT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT
        left();
      } else if (!R_IR_O && L_IR_O) { // IF RIGHT PIN TURNS OFF AND LEFT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT  
        right();
      } else {  // IF BOTH PINS ARE OFF, LIKE WHEN YOU LIFT THE CAR FROM THE TRACK
        stop();
      }  
    } 

    // REFERENCE SPEED MODE
    else if (Data.mode == 1) {
      //TODO: additional mode

      if (!set) {
        stop();
      } else {

        straightInput = Data.distance;
        straightPID.Compute();
        straightPID.SetOutputLimits(0, 255);

        if (Data.obstacle) {
          stop();
        } else if (L_IR_O && R_IR_O) {  // IF BOTH PINS ARE ON, MEANING LINE IS IN THE MIDDLE
          forward(straightOutput);
        } else if (!L_IR_O && R_IR_O) { // IF LEFT PIN TURNS OFF AND RIGHT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT
          left();
        } else if (!R_IR_O && L_IR_O) { // IF RIGHT PIN TURNS OFF AND LEFT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT  
          right();
        } else {  // IF BOTH PINS ARE OFF, LIKE WHEN YOU LIFT THE CAR FROM THE TRACK
          stop();
        }  
      }
    } 
  } 

  // IF MODE IS OUT OF RANGE
  else {
    stop();
    Data.distance = 0;
    Data.obstacle = false;
  }

  

  //printDebug();
  ending = micros();
  //Serial.println("loop:" + String(ending - starting));

  //Serial.println(Data.BuggySpeed);

}

//* Initialises All Pins to the Correct State
void PinInitialise() {
  // IR SENSORS
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // LEFT MOTOR + HALL
  pinMode(LEFT1, OUTPUT);
  pinMode(LEFT2, OUTPUT);
  pinMode(LEFT_HALL, INPUT);

  // RIGHT MOTOR + HALL
  pinMode(RIGHT1, OUTPUT);
  pinMode(RIGHT2, OUTPUT);
  pinMode(RIGHT_HALL, INPUT);

  // PWM CONTROL PINS
  pinMode(L_MOT, OUTPUT);
  pinMode(R_MOT, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_HALL), LeftHallISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_HALL), RightHallISR, RISING);
}

//* CHECK IF DISTANCE IS WITHIN LIMITS
void obstacle() {

  //Serial.println("function obstacle is ran:");

  if (now - prev >= message_interval) {
    Data.distance = sonar.ping_cm();
    prev = now;
  }

  if (Data.distance > 17 || Data.distance <= 0) {
    //Serial.println("no obs");
    Data.obstacle = false;
  } else if (Data.distance < 15) {
    //Serial.println("obs");
    Data.obstacle = true;
  }
}

//* READ AND STORE THE STATES OF THE IR SENSORS
void ReadIR() {
  L_IR_O = digitalRead(IR_LEFT);
  R_IR_O = digitalRead(IR_RIGHT);

  if (!black) {         // check the contrast color, black line on white background or white line on black background
    L_IR_O = !L_IR_O;
    R_IR_O = !R_IR_O;
  }
}

void forward(int speed) {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, speed);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, speed);

}

void left() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, 0);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, scaledTurnSpeed + TurningSpeed);

}

void right() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, scaledTurnSpeed + TurningSpeed);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, 0);

}

void stop() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, 0);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, 0);

  Data.BuggySpeed = 0;
}

//* SEND UPDATES TO RENESAS
void CheckAndSend() {
  bool changed = false;

  if (Data.obstacle != PrevData.obstacle) {
    client.print("obstacle:" + String(Data.obstacle) + '\n');
    changed = true;
  }

  if (Data.distance != PrevData.distance) {
    client.print("distance:" + String(Data.distance) + '\n');
    changed = true;

  }

  if (Data.BuggySpeed != PrevData.BuggySpeed) {
    client.print("buggyspeed:" + String(Data.BuggySpeed) + '\n');
    changed = true;
  }

  if (Data.travelled != PrevData.travelled) {
    client.print("travelled:" + String(Data.travelled) + '\n');
    changed = true;
  }

  if (changed) {
    PrevData = Data;
  }
}

void sortData(String data) {
  if (data.indexOf("enable") != std::string::npos) {
    Data.enable = !Data.enable;

  } else if (data.indexOf("speed") != std::string::npos) {  
    int sep = data.indexOf(":");
    Data.speed = data.substring(sep+1).toInt();
    //Serial.println(Data.speed);

  } else if (data.indexOf("changeMode") != std::string::npos) {
    Data.mode++;
    if (Data.mode > 1) {
      Data.mode = 0;
    }
    Serial.println(Data.mode);

  } else if (data.indexOf("set") != std::string::npos) {
    if (!set) {
    straightSetpoint = Data.distance;
    set = true;
    } else {
      set = false;
    }
  }

  //Serial.println("yes");
}

void ServerExchange() {

  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Client connected");
    } else {
      return;
    }
  }

  if (client && client.connected()) {
    if (client.available()) {
      data = client.readStringUntil('\n');
      data.trim();
      sortData(data);
    }
    CheckAndSend();
  }
}

//* INTERUPT SERVICE ROUTINE
void LeftHallISR() {
  unsigned long current = micros();
  float dt = (current - leftHall.last) / 1.0e6;
  leftHall.last = current;

  leftHall.pulse++;
  leftHall.distance = leftHall.pulse * (CIRCUM/PPR) / 100.0;

  if (dt > 0) {
    leftHall.speed = (CIRCUM/PPR) / dt;
  }
} 

//* INTERUPT SERVICE ROUTINE
void RightHallISR() {
  unsigned long current = micros();
  float dt = (current - rightHall.last) / 1.0e6;
  rightHall.last = current;

  rightHall.pulse++;
  rightHall.distance = rightHall.pulse * (CIRCUM/PPR) / 100.0;

  if (dt > 0) {
    rightHall.speed = (CIRCUM/PPR) / dt;
  }
}

void checkTimeout() {
    unsigned long current = micros();

    if (current - leftHall.last > timeout) {
        leftHall.speed = 0.0;
    }

    if (current - rightHall.last > timeout) {
        rightHall.speed = 0.0;
    }
}

//* Prints Some Debug Info over Serial
void printDebug() {

  Serial.println("------------------");

  // Serial.print("ENABLE: ");
  // Serial.println(Data.enable);

  Serial.print("LEFT IR: ");
  Serial.println(L_IR_O);
  Serial.print("RIGHT IR: ");
  Serial.println(R_IR_O);

  Serial.print("SPEED: ");
  Serial.println(String(Data.speed));

  Serial.print("DISTANCE: ");
  Serial.println(Data.distance);

  Serial.print("MODE: ");
  Serial.println(String(Data.mode));

  // Serial.print("OBS: ");
  // Serial.println(String(Data.obstacle));
  
  // Serial.print("input: ");
  // Serial.println(String(turningInput)); 

  // Serial.print("output: ");
  // Serial.println(turningOutput);

  // Serial.print("Kp: ");
  // Serial.println(String(Data.Kp));
  // Serial.print("Ki: ");
  // Serial.println(String(Data.Ki));
  // Serial.print("Kd: ");
  // Serial.println(String(Data.Kd));

  Serial.println("------------------");
}

