// LIBRARIES
#include <WiFiS3.h>       // WiFi library for arduino specifically for the ESP32-S3-MINI-1
#include <NewPing.h>      //* Library for the HC-SR04 US Rangefinder, it does the rangefinding
#include <PID_v1.h>       //* PID Library for Computing PID Output
#include <string>         // String implementation library
#include <math.h>         // math library, just used for PI
#include <vector>

#include <HUSKYLENS.h>

// WIFI SERVICE SET ID AND PASSWORD
char ssid[] = "Y13ARDUINO";
char pass[] = "123456789";

// INITIALISE SERVER ON PORT 5200 AND A CLIENT OBJECT
WiFiServer server(5200);
WiFiClient client;

//* IR sensor Pins
const int IR_LEFT = 1; // 18
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

//* LOCAL DATA STORAGE
struct LocalData{
  bool enable = false;
  int mode = 0;
  bool obstacle = false;
  double distance = 0;
  double travelled = 0;
  int speed = 0;
  double BuggySpeed = 0;
};

// ENCODER DATA STORAGE
struct EncoderData {
  volatile long pulse;
  volatile float distance;
  volatile float speed;
  volatile unsigned long last;
};

enum state {
  NORMAL,
  WAIT_LINE,
  TURNING
};

LocalData Data; //* Current Data
LocalData PrevData; //* Previous Data to Compare
state BuggyState = NORMAL;

// Initialise default encoder structs for left and right wheel encoder
EncoderData leftHall = {0, 0.0, 0.0, 0};
EncoderData rightHall = {0, 0.0, 0.0, 0};

//Pulses Per Revolution
const double PPR = 4.0;
const float CIRCUM = 6.5 * M_PI; // in cm
const int timeout = 500000;

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
double turningInput = 0; 
double turningSetpoint;
double turningOutput;

double OldTurningSetpoint;

double turningKp = 30;
double turningKi = 0.5; 
double turningKd = 12;

// REFERENCESPEED PID VARIABLES
double ReferenceSpeedInput;
double ReferenceSpeedSetpoint = 0.0; 
double ReferenceSpeedOutput;

double ReferenceSpeedKp = 0.45;
double ReferenceSpeedKi = 0.45;
double ReferenceSpeedKd = 0; 

// REFERENCEOBJECT PID VARIABLES
double ReferenceObjectInput = 15;
double ReferenceObjectSetpoint = 15; 
double ReferenceObjectOutput;

double ReferenceObjectKp = 6.5;
double ReferenceObjectKi = 2.5;
double ReferenceObjectKd = 0;

double CF;

int rightTurn;
int leftTurn;

unsigned long turnStart = 0;
bool sharpTurn = false;

unsigned long nowCam;
unsigned long prev_Cam;

unsigned long tagTimeout = 0;

// CREATES A PID OBJECT USED FOR CALCULATING PID OUTPUT
PID turningPID(&turningInput, &turningOutput, &turningSetpoint, turningKp, turningKi, turningKd, DIRECT);
PID ReferenceSpeedPID(&ReferenceSpeedInput, &ReferenceSpeedOutput, &ReferenceSpeedSetpoint, ReferenceSpeedKp, ReferenceSpeedKi, ReferenceSpeedKd, DIRECT);
PID ReferenceObjectPID(&ReferenceObjectInput, &ReferenceObjectOutput, &ReferenceObjectSetpoint, ReferenceObjectKp, ReferenceObjectKi, ReferenceObjectKd, REVERSE);

// left and right wheel speed
double LeftSpeed;
double RightSpeed;

int baseTurningSpeed = 110; // base speed for turning
int scaledTurnSpeed; // scaled speed based on user input
int TurningSpeed;

bool set = false;

HUSKYLENS huskylens;
int TagID = 0;
int lastID = 0;

void forward(int = 150);

void setup() {
  Serial.begin(115200); // debugging serial
  Wire.begin();
  Wire.setClock( 400000UL);

  WiFi.beginAP(ssid, pass); // start Access Point
  WiFi.config(IPAddress(192, 168, 1, 1)); // static IP

  server.begin(); // start the server

  turningPID.SetMode(AUTOMATIC); // changes the mode for the PID from manual to automatic, meaning itll automatically compute the output based on the difference between input and setpoint
  ReferenceSpeedPID.SetMode(AUTOMATIC);
  ReferenceObjectPID.SetMode(AUTOMATIC);

  turningPID.SetSampleTime(50);
  ReferenceSpeedPID.SetSampleTime(50);
  ReferenceObjectPID.SetSampleTime(50);

  while(!huskylens.begin(Wire)){
    Serial.println("Huskylens begin failed!");  
    delay(1000);
  }

  PinInitialise(); // Initialise all the Pins
}

void loop() {
  starting = micros(); // Loop start time
  now = millis(); // used for checking the time of the loop

  checkTimeout();

  // check for numerator 0, dividing 0 by anything is technically compiler specific, but just in case =
  if (!(leftHall.speed == 0.0 && rightHall.speed == 0.0)) {
    Data.BuggySpeed = (leftHall.speed + rightHall.speed) / 2.0; // average speed between the two wheels
  } else {
    Data.BuggySpeed = 0.0;
  }
  
  Data.travelled = (leftHall.distance + rightHall.distance) / 2.0; // average distance travelled between the two wheels

  CheckAndSend();

  // Receive and send data every 200ms to not waste computing time every loop execution
  if (now - prev_send >= 200) {
    ServerExchange();
    prev_send = now;
  }

  // if (now- prev_Cam >= 200) {
  //   ReadCamera();
  //   prev_Cam = now;
  // }

  // ON/OFF
  if (Data.enable) {

    ReadIR(); // Reads IR sensors
    obstacle(); // Reads distance and checks for obstacles

    // from testing the pid worked when the input was -1 so i made the input -1 no matter which sensor is active
    turningSetpoint = (L_IR_O - R_IR_O);

    if (abs(turningSetpoint) > 0) {
      if (turnStart == 0) {
        turnStart = millis();
      }

      if (millis() - turnStart > 175) {
        sharpTurn = true;
      }
    } else {
      turnStart = 0;
      sharpTurn = false;
    }

    if (turningSetpoint != OldTurningSetpoint) {
      turningPID.SetMode(MANUAL);
      turningOutput = 0;
      delayMicroseconds(5);
      turningPID.SetMode(AUTOMATIC);
      OldTurningSetpoint = turningSetpoint;
    }

    turningPID.Compute(); // computes the PID
    turningPID.SetOutputLimits(-255, 255);

    scaledTurnSpeed = baseTurningSpeed + Data.speed*(0.3); // scales the turning speed based on the user set speed
    TurningSpeed = constrain(turningOutput, 0, 255);

    // Serial.println(turningOutput);

    // DEFAULT MODE
    if (Data.mode == 0) {
 
      ReferenceSpeedInput = Data.BuggySpeed;
      ReferenceSpeedPID.Compute();
      ReferenceSpeedPID.SetOutputLimits(15, 70); 

      CF = 3.64;

      Data.speed = constrain(ReferenceSpeedOutput * CF, 60, 255);
     
      // Serial.println(ReferenceSpeedOutput);
    } 

    // REFERENCE OBJECT MODE
    else if (Data.mode == 1) {

      ReferenceObjectInput = Data.distance;
      ReferenceObjectPID.Compute();
      ReferenceObjectPID.SetOutputLimits(60, 240); 
      Data.speed = (int)ReferenceObjectOutput;
    }

    if (BuggyState == NORMAL) {

      if (now - prev_Cam >= 200) {
        ReadCamera();
        prev_Cam = now;
      }

      if (TagID != 0 && ((millis() - tagTimeout) > 2000)) {
        BuggyState = WAIT_LINE;
        TagID = 0;
      }
    }

    // Serial.println(TagID);

    switch (BuggyState) {
      case NORMAL:
        move();
        break;

      case WAIT_LINE:
        move();
        break;
        
      case TURNING:
        junctionTurn();
        break;
    }
  }
  
  // WHEN BUGGY IS OFF
  else {
    stop();
    Data.distance = 0;
    Data.obstacle = false;
  }

  ending = micros();
  // Serial.println("speed: " + String(straightOutput));

    if (now - prev >= 100) {
    //printDebug();
    prev = now;
  }

  // Serial.println(BuggyState);

  // Serial.println(String(ending - starting));
}

//* Initialises All Pins to the Correct State
void PinInitialise() {
  // IR SENSORS
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // LEFT MOTOR + HALL
  pinMode(LEFT1, OUTPUT);
  pinMode(LEFT2, OUTPUT);
  pinMode(LEFT_HALL, INPUT_PULLUP); // INPUT_PULLUP uses na internal pullup resistor to restor the state of the pin to HIGH, since the hall effect sensors use an open collector

  // RIGHT MOTOR + HALL
  pinMode(RIGHT1, OUTPUT);
  pinMode(RIGHT2, OUTPUT);
  pinMode(RIGHT_HALL, INPUT_PULLUP); // INPUT_PULLUP uses na internal pullup resistor to restor the state of the pin to HIGH, since the hall effect sensors use an open collector

  // PWM CONTROL PINS
  pinMode(L_MOT, OUTPUT);
  pinMode(R_MOT, OUTPUT);

  // ATTACHES INTERRUPT AND ISR TO THE ENCODER PINS AND SETS MODE, RISING = TRANSITION FROM LOW -> HIGH
  attachInterrupt(digitalPinToInterrupt(LEFT_HALL), LeftHallISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_HALL), RightHallISR, RISING);
}

//* CHECK IF DISTANCE IS WITHIN LIMITS
void obstacle() {

  //Serial.println("function obstacle is ran:");

  // ONLY PING THE US SENSOR EVERY 50ms
  if (now - prev >= message_interval) {
    Data.distance = sonar.ping_cm();
    prev = now;
  }

  // CHECKS DISTANCE, ADDS A 2CM BUFFER ZONE, BETWEEN 15-17cm
  if (Data.distance >= 15 || Data.distance <= 0) {
    Data.obstacle = false;
  } else if (Data.distance < 15) {
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

// GO FORWARD
void forward(int speed) {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, speed);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, speed);

}

// GO LEFT, USES COMPUTED TURNING SPEED AND SCALED TURNING SPEED
void left() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, 0);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, scaledTurnSpeed + TurningSpeed);

}

// GO RIGHT, USES COMPUTED TURNING SPEED AND SCALED TURNING SPEED
void right() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, scaledTurnSpeed + TurningSpeed);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, 0);
}

void sharpLeft() {

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, scaledTurnSpeed + TurningSpeed);

  // digitalWrite(LEFT1, HIGH);
  // digitalWrite(LEFT2, LOW);
  // analogWrite(L_MOT, 40);

  // delayMicroseconds(10);

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, LOW);
  analogWrite(L_MOT, 0);
}

void sharpRight() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, scaledTurnSpeed + TurningSpeed);

  // digitalWrite(RIGHT1, HIGH);
  // digitalWrite(RIGHT2, LOW);
  // analogWrite(R_MOT, 50);

  // delayMicroseconds(10);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, LOW);
  analogWrite(R_MOT, 0);
}

void junctionTurn() {

  if (TagID == 1) {

    left();
    if (R_IR_O) {
      forward(Data.speed);
      BuggyState = NORMAL;
      TagID = 0;
      tagTimeout = millis();
    }

  } else if (TagID == 2) {

    right();
    if (L_IR_O) {
      forward(Data.speed);
      BuggyState = NORMAL;
      TagID = 0;
      tagTimeout = millis();
    }
  }
}

// STOP 
void stop() {

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, 0);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, 0);

  Data.BuggySpeed = 0;
  
  ReferenceSpeedPID.SetMode(MANUAL);
  ReferenceSpeedOutput = 0.10;
  delayMicroseconds(5);
  ReferenceSpeedPID.SetMode(AUTOMATIC);
  
  ReferenceObjectPID.SetMode(MANUAL);
  ReferenceObjectOutput = 50;
  delayMicroseconds(5);
  ReferenceObjectPID.SetMode(AUTOMATIC);
}

// CHECKS WHICH VALUES IF ANY HAVE CHANGED, SENDS THEM, AND THEN CHANGES THE STATE OF PrevData TO REFLECT THE CHANGE
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

// DOES SOMETHING BASED ON THE DATA
void sortData(String data) {

  // if message is enable we want to invert the state of enable
  if (data.indexOf("enable") != std::string::npos) {
    Data.enable = !Data.enable;
  }

  // if data contains speed we want to take the substring following : out and cast to an int
  else if (data.indexOf("speed") != std::string::npos) {  
    int sep = data.indexOf(":");
    ReferenceSpeedSetpoint = data.substring(sep+1).toFloat();
  }

  // if data is changeMode we want to increment mode, and keep it within some predefined range
  else if (data.indexOf("changeMode") != std::string::npos) {
    Data.mode++;
    if (Data.mode > 2) {
      Data.mode = 0;
    }
  }

  else if (data.indexOf("kp") != std::string::npos) {
    int sep = data.indexOf(":");
    turningKp = data.substring(sep+1).toFloat();
    turningPID.SetTunings(turningKp, turningKi, turningKd);
  }

  else if (data.indexOf("ki") != std::string::npos) {
    int sep = data.indexOf(":");
    turningKi = data.substring(sep+1).toFloat();
    turningPID.SetTunings(turningKp, turningKi, turningKd);
  }

  else if (data.indexOf("kd") != std::string::npos) {
    int sep = data.indexOf(":");
    turningKd = data.substring(sep+1).toFloat();
    turningPID.SetTunings(turningKp, turningKi, turningKd);
  }
}

// CHECKS FOR CLIENTS AND IF ONE IS AVAILABLE WE WILL SEE IF DATA IS AVAILABLE TO READ FROM IT
void ServerExchange() {

  // checks if there is no client, or if the client has disconnected and will assign one if possible otherwise terminate the function since the rest of the function 
  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Client connected");
      
      client.print("kp:" + String(turningKp) + "\n");
      client.print("ki:" + String(turningKi) + "\n");
      client.print("kd:" + String(turningKd) + "\n");
    } else {
      return;
    }
  }

  // as a precaution check again if the client is connected, then check if there is data available and read it, trim it, then sort it
  if (client && client.connected()) {
    if (client.available()) {
      data = client.readStringUntil('\n');
      data.trim();
      sortData(data);
    }
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

// checks the timeout of the wheel encoders, if the wheels dont spin we dont want to keep the last recorded speed, we instead want it to be zero, so we timeout the wheels after X seconds and set the speed to 0;
void checkTimeout() {
    unsigned long current = micros();

    if (current - leftHall.last > timeout) {
        leftHall.speed = 0.0;
    }

    if (current - rightHall.last > timeout) {
        rightHall.speed = 0.0;
    }
}

int area(int x, int y) {
  return x*y;
}

void ReadCamera() {

  if (!huskylens.request())  {
    Serial.println("Request Failed.");
  } else if (!huskylens.isLearned()) {
    Serial.println("Nothing learned.");
  } else if (huskylens.available()) {
    int blockCount = huskylens.countBlocks();
    int blockArea[blockCount] = { 0 };

    std::vector<HUSKYLENSResult> blocks;
    blocks.push_back(huskylens.read());

    HUSKYLENSResult max = blocks[0];
    
    for (int i = 1; i < blockCount-1; i++) {
      blocks.push_back(huskylens.read());
      if (area(blocks[i].width, blocks[i].height) > area(max.width, max.height)) {
        max = blocks[i];
      }
    }

    TagID = max.ID;
  }
}

void move() {

  if (Data.obstacle){
    stop();
  } else if (Data.mode == 0 && ReferenceSpeedSetpoint == 0) {
    stop();
  } else if (L_IR_O && R_IR_O) {  // IF BOTH PINS ARE ON, MEANING LINE IS IN THE MIDDLE
    if (Data.mode == 0) {
      forward(Data.speed);
    } else if (Data.mode == 1 && Data.distance != 0) {
      forward(Data.speed);
    } else {
      forward();
    }
  } else if (!L_IR_O && R_IR_O) { // IF LEFT PIN TURNS OFF AND RIGHT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT
    if (sharpTurn) {
      sharpLeft();
    } else {
      left();
    }
  } else if (!R_IR_O && L_IR_O) { // IF RIGHT PIN TURNS OFF AND LEFT PIN STAYS ON, MEANING LEFT IR SENSOR IS TRIPPED, TURN LEFT  
    if (sharpTurn) {
      sharpRight();
    } else {
      right();
    }
  } else {  // IF BOTH PINS ARE OFF, LIKE WHEN YOU LIFT THE CAR FROM THE TRACK
    if (BuggyState == NORMAL) {
    //   if (Data.mode == 0) {
    //   forward(Data.speed);
    //   } else if (Data.mode == 1 && Data.distance != 0) {
    //     forward(Data.speed);
    //   } else {
    //     forward();
    //   }

    stop();
    } else if (BuggyState == WAIT_LINE) {
      if (!L_IR_O && !R_IR_O ) {
       BuggyState = TURNING;
      }
    } 
  }
}

//* Prints Some Debug Info over Serial
void printDebug() {

  Serial.println("------------------");

  // Serial.print("ENABLE: ");
  // Serial.println(Data.enable);

  // Serial.print("LEFT IR: ");
  // Serial.println(L_IR_O);
  // Serial.print("RIGHT IR: ");
  // Serial.println(R_IR_O);

  // Serial.print("SPEED: ");
  // Serial.println(String(Data.speed));

  // Serial.print("DISTANCE: ");
  // Serial.println(Data.distance);

  // Serial.print("MODE: ");
  // Serial.println(String(Data.mode));

  // Serial.print("OBS: ");
  // Serial.println(String(Data.obstacle));
  
  // Serial.print("input: ");
  // Serial.println(String(turningInput)); 

  // Serial.print("output: ");
  // Serial.println(turningOutput);

  // Serial.println("loop:" + String(ending - starting));

  // Serial.print("KP: ");
  // Serial.println(turningKp);
  // Serial.print("KI: ");
  // Serial.println(turningKi);
  // Serial.print("KD: ");
  // Serial.println(turningKd);
}