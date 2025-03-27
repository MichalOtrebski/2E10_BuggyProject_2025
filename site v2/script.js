
//////////////////////////////////

// Variables
let enable = false;
let speed = 0;
let distance = 0;
let travelled = 0;
let mode = 0;
let socket;

let isChangingSpeedSlider = false;

//////////////////////////////////

// References to IDs
const startStopBtn = document.getElementById('startStopBtn');
const speedValueElem = document.getElementById('speedValue');
const distanceElem = document.getElementById('distance');
const modeSelect = document.getElementById('changeMode');
const obstacleText = document.getElementById('obstacleText');

const slider = document.getElementById("speedslider");
const output = document.getElementById("speedSet");

//////////////////////////////////

var gaugeElement = document.getElementsByTagName('canvas')[0];

gaugeElement.setAttribute('data-value', 0);

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
            slider.value = speed;
            output.innerHTML = `Speed:${speed} cm/s`;
        }

        if (data.BuggySpeed !== undefined) {
            speed = data.speed;
            gaugeElement.setAttribute('data-value', speed);
        }

        if (data.distance !== undefined) {
            document.querySelectorAll('.distance-section p')[0].textContent = `${data.distance} cm`;
            console.log(distance);
        }

        if (data.travelled !== undefined) {
            document.querySelectorAll('.distance-section p')[1].textContent = `${data.travelled.toFixed(2)} m`;
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
    updateStartButton();
    
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
    isChangingSpeedSlider = true;  // Prevent WebSocket updates while changing
    socket.send(`speedValue:${slider.value}`);
    output.innerHTML = `Speed:${slider.value} cm/s`;
});

// User stopped changing
slider.addEventListener("change", function () {
    isChangingSpeedSlider = false;  // Allow WebSocket updates again
});

// Display Obstacle Detected Text
function updateObstacleAlert(obstacleDetected) {
    obstacleText.style.display = obstacleDetected ? 'block' : 'none';
}

// Update start/stop button state based on enable state
function updateStartButton() {
    if (enable) {
        startStopBtn.textContent = 'Stop';
        startStopBtn.style.setProperty('--default-color', 'rgb(196, 64, 64)');
        startStopBtn.style.setProperty('--hover-color', 'rgb(153, 51, 51)');
    } else {
        startStopBtn.textContent = 'Start';
        startStopBtn.style.setProperty('--default-color', 'rgb(0, 204, 102)');
        startStopBtn.style.setProperty('--hover-color', 'rgb(0, 153, 77)');
    }
}

// Update mode description text
function updateModeText(mode) {
    let modeText = "";
    switch (mode) {
        case 0:
            modeText = "Reference Speed";
            break;
        case 1:
            modeText = "Reference Object";
            break;
        default:
            modeText = "Unknown Mode";
            break;
    }
    modeSelect.textContent = `${modeText}`;
}

function updateSpeedValue() {
    if (!isChangingSpeedSlider) {
        slider.value = speed;
    }
}

updateModeText(mode); // might be redundant but havent tested
setupWebSocket();