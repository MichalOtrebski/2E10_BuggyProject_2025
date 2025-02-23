// LIBRARIES
import meter.*;                // meter Library for graphing analog values
import g4p_controls.*;         // G4P Library for TextField Objects
import controlP5.*;            // ControlP5 Library for control systems
import java.awt.Font;          // font library
import processing.net.*;       // Networking library for processing


Client arduino;                // INITIALISES A CLIENT OBJECT
String data;                   // STORES RECEIVED DATA
boolean connected = false;     // CHECKS IF WE ARE CONNECTED

ControlP5 cp5;                 // CONTROLP5 OBJECT FOR CREATING BUTTONS AND SLIDERS


// INITIALISES BUTTONS, SLIDERS, METERS, AND TEXTFIELDS
Button Start;
Button Mode;
Button SetDistance;
Slider speedSlider;

GTextField DistanceDisplay;
GLabel DistanceLabel;

GTextField DistanceTrav;
GLabel TravLabel;

Meter speedometer;

// VARIABLES
int flashInterval = 500;  // interval for flashing the screen red when theres an obstacle
int lastFlashTime = 0;    // last flash time for checking the interval

int mode = 0;             // mode that we are in
String distance = "0";
String travelled = "0";
int sliderSpeed;          // speed set by the slider
double speed = 0;         // actual buggy speed
boolean obstacle = false;
boolean showRed = false;  // tells the program when to display red when theres an obstacle, ex. show red for 500ms so itll be true, then turn false and return to normal

int btnwidth, btnheight;  // standard button height and width
boolean StartisPressed = false;  // i make the button a toggle switch, so itll be green normally and if we press it itll be red and change the text

int distColor = color(220, 150, 50);  // color fot the border of distance from US sensor
int borderwidth = 5;                  // width of said border above

void setup() {
  size(1400, 800);  // window size
  
  arduino = new Client(this,"192.168.1.1", 5200);  //connect to the arduino Access Point
  
  // CREATES A NEW THREAD FOR RUNNING THE listen() FUNCTION WHICH LISTENS TO INCOMING DATA FROM THE ARDUINO
  new Thread(new Runnable() {
    public void run() {
     listen(); 
    }
  }).start();
  
  // if the arduino is active.
  if (arduino.active()) {
    connected = true;
  }
  
  cp5 = new ControlP5(this);
  
  // button width and height based on window size
  btnwidth = width/7;
  btnheight = height/10;
  
  // just a font sused for the textfield
  Font customFont = new Font("Arial", Font.PLAIN ,24);
  
  // START/STOP BUTTON
  Start = cp5.addButton("enable")
      .setPosition(width*(0.025), width*(0.025))
      .setSize(btnwidth, btnheight)
      .setLabel("Turn ON")
      .setColorBackground(color(100, 230, 100))
      .setColorForeground(color(70, 180, 70))
      .setColorActive(color(100, 100, 100));

  Start.getCaptionLabel().setSize(20);
  
  // CHANGE MODE BUTTON
  Mode = cp5.addButton("changeMode")
      .setPosition(width*(0.025), 2 * width*(0.025) + btnheight)
      .setSize(btnwidth, btnheight)
      .setLabel("Reference Speed")
      .setColorBackground(color(150, 150, 150))
      .setColorForeground(color(120, 120, 120))
      .setColorActive(color(100, 100, 100));
     
  Mode.getCaptionLabel().setSize(20);
  
  // DISTANCE TEXTFIELD
  DistanceDisplay = new GTextField(this, width*(0.025)+ btnwidth/4, 3*width*(0.025) + 2*btnheight, btnwidth/2, btnheight/2);
  DistanceDisplay.setFont(customFont);
  DistanceDisplay.setText(distance + " cm");
  DistanceDisplay.setOpaque(true);
  DistanceDisplay.setLocalColorScheme(GCScheme.BLUE_SCHEME);
  DistanceDisplay.setTextAlign(GAlign.CENTER, GAlign.CENTER);
  DistanceDisplay.setEnabled(false);
  
  DistanceLabel = new GLabel(this,  width*(0.025) + btnwidth/3.5, 3.7*width*(0.025) + 2*btnheight, btnwidth, btnheight);
  DistanceLabel.setText("- Distance -");
  DistanceLabel.setFont(new Font("Arial", Font.BOLD, 14));
  DistanceLabel.setOpaque(false);
  DistanceLabel.setTextBold();
  DistanceLabel.setLocalColor(G4P.TEXT, color(255));
  
  // TRAVELLED DISTANCE TEXTFIELD
  DistanceTrav = new GTextField(this, width*(0.025)+ btnwidth/4, 4*width*(0.025) + 3*btnheight, btnwidth/2, btnheight/2);
  DistanceTrav.setFont(customFont);
  DistanceTrav.setText(travelled + " m");
  DistanceTrav.setOpaque(true);
  DistanceTrav.setLocalColorScheme(GCScheme.BLUE_SCHEME);
  DistanceTrav.setTextAlign(GAlign.CENTER, GAlign.MIDDLE);
  DistanceTrav.setEnabled(false);
  
  TravLabel = new GLabel(this,  width*(0.025)+ btnwidth/6, 4.7*width*(0.025) + 3*btnheight, btnwidth, btnheight);
  TravLabel.setText("Distance Travelled");
  TravLabel.setText("- Distance Travelled -");
  TravLabel.setFont(new Font("Arial", Font.BOLD, 14));
  TravLabel.setOpaque(false);
  TravLabel.setTextBold();
  TravLabel.setLocalColor(G4P.TEXT ,color(255));

  // SPEED CONTROL SLIDER
  speedSlider = cp5.addSlider("speed")
     .setPosition(2*width*(0.025)+1.25*btnwidth, width*(0.025)) // X, Y position
     .setSize(40, Math.round(width*(0.025)+2*btnheight)) // Width, Height
     .setRange(0, 100) // Min & Max values
     .setValue(sliderSpeed) // Default value
     .setColorForeground(color(40, 200, 200)) // Slider color
     .setColorBackground(color(100)) // Background color
     .setColorActive(color(0, 160, 160)); // Color when sliding
     
  // SPEEDOMETER METER
  speedometer = new Meter(this, Math.round(3*width*(0.025)+1.5*btnwidth) + 40, Math.round(width*(0.025)), false);
  
  String[] scaleLabels = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"};
  
  speedometer.setMeterWidth(350);
  speedometer.setMinScaleValue(0.0);
  speedometer.setMaxScaleValue(100);
  speedometer.setMinInputSignal(0);
  speedometer.setMaxInputSignal(100);
  speedometer.setTitle("Speed [cm/s]");
  speedometer.setScaleLabels(scaleLabels);

  speedometer.setDisplayDigitalMeterValue(true);
}

void draw() {
  background(50);  // window background color
  
  // UPDATE THE DISTANCE AND DISTANCE TRAVELLED
  DistanceDisplay.setText(distance + " cm");
  DistanceTrav.setText(travelled + " m");
  
  // if theres an obstacle flash the screen red every X ms
  if (obstacle) {
    
      // check if we should show red or default based on the interval
      if (millis() - lastFlashTime > flashInterval) {
        showRed = !showRed;
        lastFlashTime = millis();
      }
  
      // if we should show red show it, otherwise show default
      if (showRed) {
        background(255, 0, 0); // Red flash
      } else {
        background(50); // Normal background
      }
      
      // alert settings
      fill(255); // color of text, 255 = white
      textAlign(CENTER, CENTER);  // justification, centered in the window
      textSize(50);  // font size
      text("OBSTACLE ALERT!", width / 2, height / 2);
    } else {
      background(50);
  }
  
  // update the speedometer with new speed
  speedometer.updateMeter((int)speed);

  // draw the box around Distance and Distance travelled
  drawBorderedBox(width*(0.025)+ btnwidth/4 - borderwidth, 3*width*(0.025) + 2*btnheight - borderwidth, btnwidth/2 + borderwidth*2, btnheight/2 + borderwidth*2, distColor);
  drawBorderedBox(width*(0.025)+ btnwidth/4 - borderwidth, 4*width*(0.025) + 3*btnheight - borderwidth, btnwidth/2 + borderwidth*2, btnheight/2 + borderwidth*2, color(50, 150, 200));
}

// Event Handler
void controlEvent(ControlEvent event) {
  
  // if the event is from the start/sstop button, toggle between on and off
  if (event.isController() && event.getController().getName().equals("enable")) {
    StartisPressed = !StartisPressed; // Toggle state
    if (StartisPressed) {
      Start.setColorBackground(color(230, 100, 100)); // New color
      Start.setColorForeground(color(180, 70, 70));
      Start.setLabel("Turn OFF");
    } else {
      Start.setColorBackground(color(100, 230, 100)); // Default color
      Start.setColorForeground(color(70, 180, 70));
      Start.setLabel("Turn ON");
      obstacle = false;
    }
    
    SendData("enable");
  } 
  
  // if the event is from the change mode button, change the mode and update
  else if (event.isController() && event.getController().getName().equals("changeMode")) {

    mode++;
    if (mode > 1) {
      mode = 0;
    }
    
    switch(mode) {
    case 0:
      Mode.setLabel("Reference Speed");
      break;
    case 1:
      Mode.setLabel("Reference Object");
      break;
    }
    
    SendData("changeMode");
  } 
  
  // if the event is from the speed slider, convert from percentage to 8 bit and send the data
  else if (event.isController() && event.getController().getName().equals("speed")) {
    sliderSpeed = (int) event.getController().getValue();
    float bit_8 = sliderSpeed/100.0*255.0;
    SendData("speed:" + str(bit_8));
    println(str(bit_8));
  }
}

// draws a box give coords and wcolor.
void drawBorderedBox(float x, float y, float w, float h, int fillColor) {
  stroke(fillColor); // Border color
  strokeWeight(3);
  fill(fillColor); // Background color
  rect(x, y, w, h, 3.0); // Rounded rectangle
}

// receive the data from arduino, parse through it, and update values based on the data
void listen() {
  while(connected) {
    if (arduino.available() > 0) {

      data = arduino.readStringUntil('\n');
      data.trim();
      int sep = data.indexOf(":");
      String type = data.substring(0, sep);
      String value = data.substring(sep + 1);

      switch(type) {
        case "buggyspeed":
          speed = Double.parseDouble(value);
          println((int)speed);
          break;
        case "obstacle":
          obstacle = value.equals("1\n");
          break;
        case "distance":
          distance = value;
          break;
        case "travelled":
          travelled = value;
          break;
      }
    }
  }
}

// send data to arduino
void SendData(String message) {
  if (connected && arduino.active()) {
    arduino.write(message + '\n');
    println(message);
  }
}
