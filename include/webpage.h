#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* html = R"=====(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Arduino Control Dashboard</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>

    <!-- OBSTACLE DETECTED Text -->
    <div id="obstacleText">OBSTACLE DETECTED</div>

    <h1>Arduino Control Dashboard</h1>

    <!-- Control Panel -->
    <div id="controlPanel">
        <button id="startStopBtn" class="button">Start</button>
        <div id="modeSelector">
            <button id="changeMode" class="button2">Change Mode</button>
            </select>
        </div>
    </div>

    <!-- Speedometer -->
    <div id="speedometer">
        <div id="speedValue">0</div>
    </div>

    <!-- Distance -->
    <div id="distance">Distance: 0 cm</div>

    <!-- Mode Display -->
    <div style="margin-top: 10px;" id="modeText">Default Mode</div>

    <!-- Seed Control -->
    <div style="margin-top: 25px;" class="slidecontainer">
        <input type="range" min="0" max="100" value="0" class="slider" id="speedRange">
    </div>

    <div style="margin-top: 15px;" id="speedSet">Speed: 0%</div>

    <!-- PID Constants Control -->
    <div style="margin-top: 25px;"class="slidecontainer">
        <input type="range" id="Kp" class="slider2" min="0" max="10" value="0" step="0.1">
        <span id="Kp_value" class="value">Kp: 0.0</span>
    </div>

    <div class="slidecontainer">
        <input type="range" id="Ki" class="slider2" min="0" max="1" value="0" step="0.01">
        <span id="Ki_value" class="value">Ki: 0.00</span>
    </div>

    <div class="slidecontainer">
        <input type="range" id="Kd" class="slider2" min="0" max="1" value="0" step="0.01">
        <span id="Kd_value" class="value">Kd: 0.00</span>
    </div>

    

    <script src="script.js"></script>

</body>
</html>

)=====";

const char* css = R"=====(

body {
    font-family: sans-serif;
    text-align: center;
    margin: 0;
    padding: 0;
    background-color: #f0f0f0;
}

#obstacleText {
    font-size: 40px;
    font-weight: bold;
    color: red;
    display: none;
}

h1 {
    margin-top: 20px;
    font-size: 25px;
}

#modeSelector, #controlPanel {
    margin-top: 30px;
    font-size: 20px;
}


/* BUTTONS */
.button {
    padding: 10px 20px;
    font-size: 18px;
    margin: 10px;
    cursor: pointer;
    border: none;
    border-radius: 5px;
    background-color: #45a049;
    color: white;
    transition: background-color 0.3s;
}

.button2 {
    padding: 10px 20px;
    font-size: 18px;
    margin: 10px;
    cursor: pointer;
    border: none;
    border-radius: 5px;
    background-color: #818181;
    color: white;
    transition: background-color 0.3s;
}

/* Hover Feature */
.button:hover {
    background-color: #38833c;
}

.button2:hover {
    background-color: #555555;
}

#speedometer {
    margin-top: 20px;
    width: 100px;
    height: 100px;
    border: 10px solid #4CAF50;
    border-radius: 50%;  /* rounds the border, 50% = circle */
    display: inline-block;
    position: relative;
}

#speedValue {
    font-size: 30px;
    color: #4CAF50;
    position: absolute;
    top: 35%;
    left: 35%;
}

#distance {
    font-size: 25px;
    color: #333;
    margin-top: 30px;
}

#mode, #modeText {
    font-size: 25px;
    color: #333;
}

/* just a width for the slider container */
.slidecontainer {
    width: 100%;
}

/* SPEED SLIDER */
.slider:hover {
    opacity: 1;
}

.slider {
    -webkit-appearance: none;
    appearance: none;
    width: 50%;
    height: 10px;
    background: #d3d3d3;
    outline: none; 
    opacity: 0.7; 
    -webkit-transition: .2s;
    transition: opacity .2s;
}



/* Webskit slider to override default, webkit for certain browsers, and moz for mozilla firefox */
.slider::-webkit-slider-thumb {
    -webkit-appearance: none; /* Override default look */
    appearance: none;
    width: 15px;
    height: 25px;
    background: #177bff;
    cursor: pointer;
}

.slider::-moz-range-thumb {
    width: 25px;
    height: 25px;
    background: #177bff;
    cursor: pointer;
}


/* PID SLIDERS */
.slider2:hover {
    opacity: 1; /* Fully shown on mouse-over */ 
}

.slider2 {
    -webkit-appearance: none;
    appearance: none;
    width: 40%;
    height: 5px;
    background: #d3d3d3;
    outline: none; 
    opacity: 0.7; 
    -webkit-transition: .2s;
    transition: opacity .2s;
}

)=====";

const char* js = R"=====(

//////////////////////////////////

// Variables
let enable = false;
let speed = 0;
let distance = 0;
let mode = 0;
let socket;

let kp;
let ki;
let kd;

let isChangingSpeedSlider = false;

let BuggySpeed;

//////////////////////////////////

// References to IDs
const startStopBtn = document.getElementById('startStopBtn');
const speedValueElem = document.getElementById('speedValue');
const distanceElem = document.getElementById('distance');
const modeSelect = document.getElementById('changeMode');
const modeTextElem = document.getElementById('modeText');
const obstacleText = document.getElementById('obstacleText');

const slider = document.getElementById("speedRange");
const output = document.getElementById("speedSet");

const ProSlider = document.getElementById('Kp');
const ProVal = document.getElementById('Kp_value');

const IntSlider = document.getElementById('Ki');
const IntVal = document.getElementById('Ki_value');

const DerSlider = document.getElementById('Kd');
const DerVal = document.getElementById('Kd_value');

//////////////////////////////////

// Set up WebSocket connection
function setupWebSocket() {
    socket = new WebSocket("ws://" + window.location.hostname + ":81");
    
    // console logging
    socket.onopen = () => {
        console.log("Connected to WebSocket server.");
    };

    // Incoming WebSocket messages
    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.speed !== undefined) {
            speed = Math.round(data.speed/255*100);
            updateSpeedValue();
        }

        if (data.buggyspeed !== undefined) {
            BuggySpeed = data.buggyspeed;
            speedValueElem.textContent = data.buggyspeed;
        }

        if (data.distance !== undefined) {
            distance = data.distance;
            distanceElem.textContent = `Distance: ${distance} cm`;
        }

        if (data.mode !== undefined) {
            mode = data.mode;
            updateModeText(mode); // Update mode description based on mode
        }

        if (data.obstacle !== undefined) {
            updateObstacleAlert(data.obstacle);
            //toogle obstacle detection
        }

        if (data.enable !== undefined) {
            enable = data.enable;
            console.log("enable = " + enable);
            updateStartButton();    
            /*update start button base on message, syncs multiple clients so that they all have the same state for start and stop*/
        }

        // PID
        if (data.Kp !== undefined) {
            kp = data.Kp;
            ProVal.innerHTML = `Kp: ${kp.toFixed(1)}`;
        }

        if (data.Ki !== undefined) {
            ki = data.Ki;
            IntVal.innerHTML = `Ki: ${ki.toFixed(2)}`;
        }
        
        if (data.Kd !== undefined) {
            kd = data.Kd;
            DerVal.innerHTML = `Kd: ${kd.toFixed(2)}`;
        }

    };

    // Handle WebSocket close
    socket.onclose = () => {
        console.log("Disconnected from WebSocket server.");
    };

    // Handle WebSocket errors
    socket.onerror = (error) => {
        console.log("WebSocket error: ", error);
    };
}

// Toggle Start/Stop Button
startStopBtn.addEventListener('click', () => {

    console.log("Sending command:", enable ? "start" : "stop");
    
    if (enable) {
        socket.send("stop");
    } else {
        socket.send("start");
    }

});

// listen to clicks on change mode
modeSelect.addEventListener('click', () => {
    console.log("Sending command:", "changeMode");
    socket.send("changeMode");
});

// listen for change to speed slider
slider.addEventListener('input', () => {
    isUserChangingSpeed = true;
    socket.send(`speedValue:${slider.value}`);
    output.innerHTML = `Speed:${slider.value}%`;
});

// User stopped changing
slider.addEventListener("change", function () {
    isChangingSpeedSlider = false;  // Allow WebSocket updates again
});

// PID
ProSlider.addEventListener('input', () => {
    socket.send(`KpValue:${ProSlider.value}`);
    ProVal.innerHTML = `Kp: ${ProSlider.value.toFixed(2)}`;
});

// PID
IntSlider.addEventListener('input', () => {
    socket.send(`KiValue:${IntSlider.value}`);
    IntVal.innerHTML = `Ki: ${IntSlider.value.toFixed(2)}`;
});

// PID
DerSlider.addEventListener('input', () => {
    socket.send(`KdValue:${DerSlider.value}`);
    DerVal.innerHTML = `Kd: ${DerSlider.value.toFixed(2)}`;
});

// Display Obstacle Detected Text
function updateObstacleAlert(obstacle) {
    obstacleText.style.display = obstacle ? 'block' : 'none';
}

// Update start/stop button state based on enable state
function updateStartButton() {
    if (enable) {
        startStopBtn.textContent = 'Stop';
    } else {
        startStopBtn.textContent = 'Start';
    }
}

// Update mode description text
function updateModeText() {
    let modeText = "";
    switch (mode) {
        case 0:
            modeText = "Mode 0: Default Mode";
            break;
        case 1:
            modeText = "Mode 1: Reference Speed";
            break;
        case 2:
            modeText = "Mode 2: Reference Object";
            break;
        case 3:
            modeText = "Mode 3: -----------------";
            break;
        default:
            modeText = "Unknown Mode";
            break;
    }
    modeTextElem.textContent = `${modeText}`;
}

function updateSpeedValue() {
    if (!isChangingSpeedSlider) {
        slider.value = speed;
    }
}

setupWebSocket();


)=====";

#endif