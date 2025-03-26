
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

var gaugeElement = document.getElementsByTagName('canvas')[0];

gaugeElement.setAttribute('data-value', x);

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
            speed = data.speed;
            speedValueElem.textContent = speed;
            slider.value = speed;
        }

        if (data.distance !== undefined) {
            distance = data.distance;
            distanceElem.textContent = `Distance: ${distance} cm`;
        }

        if (data.mode !== undefined) {
            mode = data.mode;
            updateModeText(mode); // Update mode description based on mode
        }

        if (data.obstacleDetected !== undefined) {
            updateObstacleAlert(data.obstacleDetected);
            //toogle obstacle detection
        }

        if (data.enable !== undefined) {
            enable = data.enable;
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
    socket.send(`speedValue:${slider.value}`);
    output.innerHTML = `Speed:${slider.value}%`;
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
function updateObstacleAlert(obstacleDetected) {
    obstacleText.style.display = obstacleDetected ? 'block' : 'none';
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
function updateModeText(mode) {
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



updateModeText(mode); // might be redundant but havent tested
setupWebSocket();