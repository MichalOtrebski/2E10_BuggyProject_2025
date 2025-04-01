/* #region LIBRARIES */
#include <NewPing.h>      //* Library for the HC-SR04 US Rangefinder, it does the rangefinding
#include <PID_v1.h>       //* PID Library for Computing PID Output
#include <PacketSerial.h> //* Library for Communication Between ESP32 and Renesas

#include <functional>     //* Tools for Function Objecst, Lambdas and std::funciton Wrappers
#include <unordered_map>  //* Hash Table-Based Container Library
#include <string>         //* String implementation library
#include <math.h>         //* math library, just used for PI
#include <vector>         //* Vector Library for Dynamic Arrays
#include <queue>

#include <HUSKYLENS.h>    //* Huskylens Library for Tag Regonition

/* #endregion */

/* #region PINS */

//* IR sensor Pins
const int IR_LEFT = 1; // 1
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

/* #endregion */

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
  long loop = 0;
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

//* Packet for Sending Command and value Pairs
template<typename T>
struct __attribute__((packed)) DataPacket {
  char command[4];
  T value;

  DataPacket(const char cmd[4], T val) : value(val) {
    memcpy(command, cmd, 4);  // Copy the command into the structure
  }
};

/* #endregion */

/* #region VARIABLES */

LocalData Data; //* Current Data
LocalData PrevData; //* Previous Data to Compare
state BuggyState = NORMAL;

// Initialise default encoder structs for left and right wheel encoder
EncoderData leftHall = {0, 0.0, 0.0, 0};
EncoderData rightHall = {0, 0.0, 0.0, 0};

double leftLast = 0;
double rightLast = 0;
unsigned long lastOdometryTime = 0;

PacketSerial serialPacket;  //* Packet Serial object

std::queue<unsigned long> timeAverage;
std::queue<double> speedAverage;

int speedSampling = 4;
int speedSum = 0;
unsigned long speedPrev = 0;

int quantity = 10;
int sum = 0;

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
unsigned long prev = 0;
unsigned long loopPrev = 0;

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

unsigned long prevSpeed = 0;

// left and right wheel speed
double LeftSpeed;
double RightSpeed;

int baseTurningSpeed = 110; // base speed for turning
int scaledTurnSpeed; // scaled speed based on user input
int TurningSpeed;

bool set = false;

HUSKYLENS huskylens;
int TagDistance = 0;
int CameraConstant = 0;

bool hardStop = true;

double x = 0, y = 0, theta = 0;

// CREATES A PID OBJECT USED FOR CALCULATING PID OUTPUT
PID turningPID(&turningInput, &turningOutput, &turningSetpoint, turningKp, turningKi, turningKd, DIRECT);
PID ReferenceSpeedPID(&ReferenceSpeedInput, &ReferenceSpeedOutput, &ReferenceSpeedSetpoint, ReferenceSpeedKp, ReferenceSpeedKi, ReferenceSpeedKd, DIRECT);
PID ReferenceObjectPID(&ReferenceObjectInput, &ReferenceObjectOutput, &ReferenceObjectSetpoint, ReferenceObjectKp, ReferenceObjectKi, ReferenceObjectKd, REVERSE);

/* #endregion */

/* #region FUNCTION PROTOTYPES*/

template <typename T>
void SendUpdate(const char command[4], T value);
void onPacketReceived(const uint8_t* buffer, size_t size);

void CheckAndSend();

void obstacle();
void ReadIR();

void move();
void forward(int = 150);
void left();
void sharpLeft();
void right();
void sharpRight();
void junction();
void stop();

void LeftHallISR();
void RightHallISR();
void SpeedAndDistance();
void checkTimeout();

void ReadCamera();
void area();

void boot();
void PinInitialise();

void printDebug();

/* #endregion */

void setup() {
  Serial.begin(230400);
  Serial2.begin(115200);
  serialPacket.setStream(&Serial2);
  serialPacket.setPacketHandler(&onPacketReceived);

  Wire.begin();
  Wire.setClock(400000UL);

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

  boot();
}

void loop() {
  starting = micros();
  now = millis();
  serialPacket.update();

  checkTimeout();

  SpeedAndDistance();

  CheckAndSend();

  // ON/OFF
  if (Data.enable) {

    ReadIR();
    obstacle();

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

    if (BuggyState == NORMAL) {

      if (now - prev_Cam >= 200 && ((millis() - tagTimeout) > 2000)) {
        ReadCamera();
        prev_Cam = now;
      }

      if (Data.TagID == 1 || Data.TagID == 2) {
        BuggyState = WAIT_LINE;
      }
    }

    if (Data.TagID == 3) {
      
      // int apparentWidth = result.width;

      // int distance = (ActualWidth * focal) / apparentWidth;

      // int something = map(distance, 10, 50, 15, ReferenceSpeedSetpoint);

      // Serial.println(something);

    } else if (Data.TagID == 4) {

      Data.speed = 30;
      SendUpdate("SPD", Data.speed);
      Data.TagID = 0;
    }

    // REFERENCE SPEED MODE
    if (Data.mode == 0) {

      ReferenceSpeedInput = Data.BuggySpeed;
      ReferenceSpeedPID.Compute();
      ReferenceSpeedPID.SetOutputLimits(15, 70); 

      CF = 3.64;

      Data.speed = constrain(ReferenceSpeedOutput * CF, 50, 255);
    
    } 

    // REFERENCE OBJECT MODE
    else if (Data.mode == 1) {
      
      ReferenceObjectInput = Data.distance;
      ReferenceObjectPID.Compute();
      ReferenceObjectPID.SetOutputLimits(60, 240); 
      Data.speed = (int)ReferenceObjectOutput;
    } 

    switch (BuggyState) {
      case NORMAL:
        move();
        break;

      case WAIT_LINE:
        move();
        break;
        
      case TURNING:
        junction();
        break;
    }
  } 

  // IF MODE IS OUT OF RANGE
  else {
    stop();
    Data.distance = 0;
    Data.obstacle = false;
  }



  if (now - prev >= 100) {
    //printDebug();
    prev = now;
  }


  ending = micros();

  timeAverage.push(ending - starting);
  sum += ending - starting;

  if (timeAverage.size() > quantity) {
    sum -= timeAverage.front();
    timeAverage.pop();
  }

  if (millis() - loopPrev >= 300) {
    Data.loop = sum / quantity;
    loopPrev = millis();
  }
}

//* EVENT HANDLER FOR PACKETSERIAL
void onPacketReceived(const uint8_t* buffer, size_t size) {

  char command[4] = {};
  memcpy(command, buffer, 4);

  //* Hashmap for finding which command was sent, and then using the associated lambda function to assign the value.
  static const std::unordered_map<std::string, std::function<void(const uint8_t*)>> commandMap = {

    {"ENA", [](const uint8_t* buf) { Data.enable = reinterpret_cast<const DataPacket<bool>*>(buf)->value; }},
    {"MOD", [](const uint8_t* buf) { Data.mode = reinterpret_cast<const DataPacket<int>*>(buf)->value; }},
    // {"OBS", [](const uint8_t* buf) { Data.obstacle = reinterpret_cast<const DataPacket<bool>*>(buf)->value; }},
    // {"DIS", [](const uint8_t* buf) { Data.distance = reinterpret_cast<const DataPacket<long>*>(buf)->value; }},
    {"SPD", [](const uint8_t* buf) { ReferenceSpeedSetpoint = reinterpret_cast<const DataPacket<int>*>(buf)->value; }},
    {"QRY", [](const uint8_t* buf) { Serial.println("QRY RECEIVED"); }}
  };

  auto it = commandMap.find(command);
  it->second(buffer);
}

//* CHECK FOR CHANGES IN VALUES AND IF THEY WERE THEN SEND THE UPDATE TO RENESAS
template <typename T>
void SendUpdate(const char command[4], T value) {
  DataPacket<T> packet(command, value);
  serialPacket.send((uint8_t*)&packet, sizeof(DataPacket<T>));
}

//* CHECK IF DISTANCE IS WITHIN LIMITS
void obstacle() {

  if (now - prev >= message_interval) {
    Data.distance = sonar.ping_cm();
    prev = now;
  }

  if (Data.distance > 20 || Data.distance <= 0) {
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

void forward(int speed) {
  hardStop = true;

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, speed);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, speed);
}

void left() {

  hardStop = true;

  digitalWrite(LEFT1, LOW);
  digitalWrite(LEFT2, HIGH);
  analogWrite(L_MOT, 0);

  digitalWrite(RIGHT1, LOW);
  digitalWrite(RIGHT2, HIGH);
  analogWrite(R_MOT, scaledTurnSpeed + TurningSpeed);

}

void right() {

  hardStop = true;

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

void junction() {

  if (Data.TagID == 1) {

    if (!L_IR_O && !R_IR_O) {
      sharpLeft();
    } else if (L_IR_O) {
      forward(Data.speed);
    }   

    if (R_IR_O && !L_IR_O) {
      move();
      BuggyState = NORMAL;
      Data.TagID = 0;
      tagTimeout = millis();
    }

  } else if (Data.TagID == 2) {
    if (!L_IR_O && !R_IR_O) {
      sharpRight();
    } else if (R_IR_O) {
      forward(Data.speed);
    } 
    if (L_IR_O && !R_IR_O) {
      move();
      BuggyState = NORMAL;
      Data.TagID = 0;
      tagTimeout = millis();
    }
  }
}

void stop() {

  if (hardStop) {
    digitalWrite(LEFT1, HIGH);
    digitalWrite(LEFT2, LOW);
    analogWrite(L_MOT, 60);

    digitalWrite(RIGHT1, HIGH);
    digitalWrite(RIGHT2, LOW);
    analogWrite(R_MOT, 60);

  delay(5);
  hardStop = false;
  }

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

//* SEND UPDATES TO ESP
void CheckAndSend() {
  bool changed = false;

  if (Data.obstacle != PrevData.obstacle) {
    SendUpdate("OBS", Data.obstacle);
    changed = true;
  }

  if (Data.distance != PrevData.distance) {
    SendUpdate("DIS", Data.distance);
    changed = true;
  }

  if (Data.BuggySpeed != PrevData.BuggySpeed) {
    SendUpdate("BSP", Data.BuggySpeed);
    changed = true;
  }  

  if (Data.TagID != PrevData.TagID) {
    SendUpdate("TAG", Data.TagID);
    changed = true;
  }

  if (Data.travelled != PrevData.travelled) {
    SendUpdate("TRV", Data.travelled);
    changed = true;
  }

  if (Data.loop != PrevData.loop) {
    SendUpdate("LOP", Data.loop);
    changed = true;
  }

  // if (Data.BuggySpeed == 0) {
  //   SendUpdate("BSP", 0.0);
  // }

  if (changed) {
    PrevData = Data;
  }
}

//* Resend Initial Values Upon Renesas Reset
void boot() {
  SendUpdate("ENA", Data.enable);
  SendUpdate("MOD", Data.mode);
  SendUpdate("OBS", Data.obstacle);
  SendUpdate("DIS", Data.distance);
  SendUpdate("SPD", Data.speed);
  SendUpdate("BSP", Data.BuggySpeed);
  SendUpdate("TAG", Data.TagID);
  SendUpdate("TRV", Data.travelled);
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

void SpeedAndDistance() {

  // check for numerator 0, dividing 0 by anything is technically compiler specific, but just in case =
  if ((millis() - prevSpeed) > 200) {
    Serial.println("works");

    if (!(leftHall.speed == 0.0 && rightHall.speed == 0.0)) {
      Data.BuggySpeed = (leftHall.speed + rightHall.speed) / 2.0; // average speed between the two wheels
    } else {
      Data.BuggySpeed = 0.0;
    }

    prevSpeed = millis();
  }
  
  Data.travelled = (leftHall.distance + rightHall.distance) / 2.0; // average distance travelled between the two wheels
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

    Data.TagID = max.ID;








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
      if (Data.mode == 0) {
      forward(Data.speed);
      } else if (Data.mode == 1 && Data.distance != 0) {
        forward(Data.speed);
      } else {
        forward();
      }

    // stop();
    } else if (BuggyState == WAIT_LINE) {
      if (!L_IR_O && !R_IR_O ) {
        // stop();
       BuggyState = TURNING;
      }
    } 
  }
}

void odometry() {
  if (millis() - lastOdometryTime > 100) {
    float currentleft = leftHall.distance;
    float currentright = rightHall.distance;

    float dL = currentleft - leftLast;
    float dR = currentright - rightLast;

    leftLast = currentleft;
    rightLast = currentright;

    float d = (dL + dR) / 2.0;
    float dTheta = (dR - dL) / 13;

    x += d * cos(theta + dTheta / 2.0);
    y += d * sin(theta + dTheta / 2.0);
    theta += dTheta;

    SendUpdate("POX", x);
    SendUpdate("POY", y);

    lastOdometryTime = millis();
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

  // Serial.print("Kp: ");
  // Serial.println(String(Data.Kp));
  // Serial.print("Ki: ");
  // Serial.println(String(Data.Ki));
  // Serial.print("Kd: ");
  // Serial.println(String(Data.Kd));

  Serial.println("------------------");
}

