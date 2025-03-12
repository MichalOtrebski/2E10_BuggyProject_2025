import meter.*;
import g4p_controls.*;
import controlP5.*;
import java.awt.Font;
import processing.net.*;

Client arduino;
String data;
boolean connected = false;

ControlP5 cp5;

Button Start;
Button Mode;
Button SetDistance;
Slider speedSlider;

GTextField DistanceDisplay;
GLabel DistanceLabel;

GTextField DistanceTrav;
GLabel TravLabel;

Meter speedometer;

int flashInterval = 500;
int lastFlashTime = 0;

boolean isOn = false;
int mode = 0;
String distance = "0";
String travelled = "0";
int sliderSpeed;
double speed = 0;
boolean obstacle = false;
boolean showRed = false;

int btnwidth, btnheight;
boolean StartisPressed = false;
boolean SetisPressed = false;

int distColor = color(220, 150, 50);
int borderwidth = 5;

void setup() {
  size(1400, 800);
  
  arduino = new Client(this,"192.168.1.1", 5200);
  
  new Thread(new Runnable() {
    public void run() {
     listen(); 
    }
  }).start();
  
  if (arduino.active()) {
    connected = true;
  }
  
  cp5 = new ControlP5(this);
  
  btnwidth = width/7;
  btnheight = height/10;
  
  Font customFont = new Font("Arial", Font.PLAIN ,24);
  
  Start = cp5.addButton("enable")
      .setPosition(width*(0.025), width*(0.025))
      .setSize(btnwidth, btnheight)
      .setLabel("Turn ON")
      .setColorBackground(color(100, 230, 100))
      .setColorForeground(color(70, 180, 70))
      .setColorActive(color(100, 100, 100));

  Start.getCaptionLabel().setSize(20);
  
 
  Mode = cp5.addButton("changeMode")
      .setPosition(width*(0.025), 2 * width*(0.025) + btnheight)
      .setSize(btnwidth, btnheight)
      .setLabel("Reference Speed")
      .setColorBackground(color(150, 150, 150))
      .setColorForeground(color(120, 120, 120))
      .setColorActive(color(100, 100, 100));
     
  Mode.getCaptionLabel().setSize(20);
  
  
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

  speedSlider = cp5.addSlider("speed")
     .setPosition(2*width*(0.025)+1.25*btnwidth, width*(0.025)) // X, Y position
     .setSize(40, Math.round(width*(0.025)+2*btnheight)) // Width, Height
     .setRange(0, 100) // Min & Max values
     .setValue(sliderSpeed) // Default value
     .setColorForeground(color(40, 200, 200)) // Slider color
     .setColorBackground(color(100)) // Background color
     .setColorActive(color(0, 160, 160)); // Color when sliding
     
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
  background(50);
  
  //println(sliderSpeed);
  DistanceDisplay.setText(distance + " cm");
  DistanceTrav.setText(travelled + " m");
  
  if (obstacle) {
      if (millis() - lastFlashTime > flashInterval) {
        showRed = !showRed;
        lastFlashTime = millis();
      }
  
      if (showRed) {
        background(255, 0, 0); // Red flash
      } else {
        background(50); // Normal background
      }
  
      fill(255);
      textAlign(CENTER, CENTER);
      textSize(50);
      text("OBSTACLE ALERT!", width / 2, height / 2);
    } else {
      background(50);
  }
  
  speedometer.updateMeter((int)speed);   

  drawBorderedBox(width*(0.025)+ btnwidth/4 - borderwidth, 3*width*(0.025) + 2*btnheight - borderwidth, btnwidth/2 + borderwidth*2, btnheight/2 + borderwidth*2, distColor);
  drawBorderedBox(width*(0.025)+ btnwidth/4 - borderwidth, 4*width*(0.025) + 3*btnheight - borderwidth, btnwidth/2 + borderwidth*2, btnheight/2 + borderwidth*2, color(50, 150, 200));
}

void controlEvent(ControlEvent event) {
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
    
  } else if (event.isController() && event.getController().getName().equals("changeMode")) {

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
    
    if (mode == 1) {
      //showButton();
    } else{
      //hideButton();
    }
    
    SendData("changeMode");
    
  } else if (event.isController() && event.getController().getName().equals("SetDistance")) {
    SetisPressed = !SetisPressed;
    if (SetisPressed) {
      SetDistance.setColorBackground(color(230, 100, 100));
      SetDistance.setColorForeground(color(180, 70, 70));
      distColor = color(230, 100, 100);
    } else {
      SetDistance.setColorBackground(color(220, 160, 40));
      SetDistance.setColorForeground(color(180, 120, 0));
      distColor = color(220, 150, 50);
    }
    
    SendData("set");
    
  } else if (event.isController() && event.getController().getName().equals("speed")) {
    sliderSpeed = (int) event.getController().getValue();
    float bit_8 = sliderSpeed/100.0*255.0;
    SendData("speed:" + str(bit_8));
    println(str(bit_8));
  }
}

//void showButton() {
//  if (SetDistance == null) { // Only add if it doesn't exist
//    SetDistance = cp5.addButton("SetDistance")
//                     .setPosition(2*width*(0.01875) + btnwidth, 2*width*(0.025) + btnheight*1.25)
//                     .setSize(btnwidth/4, btnheight/2)
//                     .setColorBackground(color(220, 160, 40))
//                     .setColorForeground(color(180, 120, 0))
//                     .setColorActive(color(100, 100, 100))
//                     .setLabel("Set");
//  }
//}

//void hideButton() {
//  if (SetDistance != null) {
//    cp5.remove("SetDistance"); // Remove button
//    SetDistance = null; // Set to null so we can re-add it later
//    distColor = color(220, 150, 50);
//    SetisPressed = false;
//    SendData("set");
//  }
//}

void drawBorderedBox(float x, float y, float w, float h, int fillColor) {
  stroke(fillColor); // Border color
  strokeWeight(3);
  fill(fillColor); // Background color
  rect(x, y, w, h, 3.0); // Rounded rectangle
}

void listen() {
  while(connected) {
    if (arduino.available() > 0) {
      //println("connected");
        
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

void SendData(String message) {
  if (connected && arduino.active()) {
    arduino.write(message + '\n');
    println(message);
  }
}
