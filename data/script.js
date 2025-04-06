
//////////////////////////////////

// Variables
let enable = false;
let speed = 0;
let buggyspeed = 0;
let distance = 0;
let travelled = 0;
let mode = 0;
let socket;
let id = 0;
let obstacleDetected = false;
let badspeed = 0;

let minX = Infinity;
let maxX = -Infinity
let minY = Infinity;
let maxY = -Infinity;

let x = 0;
let y = 0;

const alpha = 0.5;
const padding = 20;

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

const imageElements = document.querySelectorAll('.images');

let trackPoints = [];

/* #region Images */

const images = [

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAABZklEQVR4nO3RwWkDARDFUJeWbtZl5Jim/RVMKggEot2VYI4Dw7zH4wK9juPzdRxf/31HPX4w9nzynlBEGAvFh7FQfBgLxYexUHwYC8WHsVB8GAvFh7FQfBgLxYexUHwYC8WHsVB8GAvFh7FQfBgLxYexUHwYuzuKEWN3RTFj7G4oZ8DYXVDOhLGro5wRY1dFOTPGroryF3EcH7995Hvnv+++bATiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiikBcEYgrAnFFIK4IxBWBuCIQVwTiihuAfANk+xFHMdUshwAAAABJRU5ErkJggg==",

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAACaklEQVR4nO3csWoUURSH8RGxT6FFKit7wULtLESEgEUgPsI9ZwbDFmqRzgeI4DOY0tI0IWBAsbaztBfBTgTleEICNnazzmb/d+73wdS5yW/PZDe5d7puTWXfP8hSbq/r69M/GOH+M83u8YMRwfjjnoAIYQAihgGIGAYgYhiAiGEAIoYBiBgGIGIYgIhhACKGAYgYBiBiGICIYQAihgGIGAYgYhjnIO5Ps+/vV3OVcjeH4WaWcrWbG0btV5h9D7OjNNtLsxtguBiQ+4d0f5Rdd4nJcKnJ+Xh2W+M25VLT8ivNnk0+LfzO8P+dloMs5QoYLjUtb3Jn5zLT4VIor5gS17qylG1QXGhKzL7mYrEBikvdul7yjsulQH7k7u41UFznSvfnsp9NqvvjYt9vpdkQZodhFktNidmnSUGmRKl593v2/a1w/7LU913K5ioW1Pw/qHIYrof7tyVeiI8nB5kCpeYJ+VuaPVnitvWiW1Wtb3LIxWIjzH6PBHm92kU1vg0ozD6PBHm78kW1vFEuzN6PBHl3IQtrdStpuJ+MAnE/ubDFtbjZOpRBWjyOEOogrR3YiRpAWjrSFrWAtHLoM2oCaeFYdNQGMvcHB0SNIHN+tEbUCjLXh89EzSBzfDxT1A5yVpbyMN3vdDMo5gAypwIQrQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQsQARCxCxABELELEAEQsQscJsP8yOR1z7614zERERUddUp0GqheoX+IPyAAAAAElFTkSuQmCC",

    "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAAAsTAAALEwEAmpwYAAAChklEQVR4nO3aoYtVQRTH8RHtIrLJYLKrSZvBIKhYXPwP5vzue2wSN1gWzAuC3Wa0aRHBBQ1mg9F/YIPJonLmLPclFyz37byd3935feGUl+bw4YZ376SkjhU5346cHx7/VTUrzO448FsoRCAFCKGQgRShcPQviFAIQYRCCCIUQhChEIIIhRBEKIQgQiEEEQohiFAIQYRCCCIUQhChEIIIhRCEFiV2drZisbi++gI3DHdnM8DTk4JQoITZtQCeu9kHN/tZY6ky8zl1lEjpXJg9cuBL6+VL7ygB3HCzr60XLr2jrJ4K4JkDf1ovWnpHib29C272uvVyZaZTFSW2t8+72dvWS5WZTzUUB161XqacFRDgwcmeDrPHrRcpZ2DqYAzDJTc7bL1MmflUwRhzs5etlykzn2oYsVxeduBX64XKjKcaxgrEbLf1QmXGUxVjzM2+rXUQM3ezdwEMMQz3u325iIoYYXZlTYwfkfPN1Pvrd1TEWB0s5ydrYBzGYnE19f6BCpUxxtzsxdTDBLBMvX/CxQYwxhx4M/Hp+Bs5X0w9X3LAhjDG3Oz9xAN9Tz1fA8IGMcbc7NPEJ+Rz6vWiHDaMMebAwcSDHaQer5LiFDDGBAIeDIGAC0Mg4MIQCLgwBAIuDIGAC0Mg4MIQCLgwxnr/H+JMGL2DOBtG1yDArcj5XmKrVxDaBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMgSCFkCIUsgZAmELIGQJRCyBEKWQMhys303+zhh9lufWSmllFJKqfTfjgDXtYbk+I8vyQAAAABJRU5ErkJggg==",

    "data:image/png;base64,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",

    "data:image/png;base64,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"
];

/* #endregion */

//////////////////////////////////

var gauge = new RadialGauge({
    renderTo: 'meter',
    width: 300,
    height: 300,
    title: "Speed",
    units: "cm/s",
    minValue: 0,
    maxValue: 50,
    majorTicks: [
        "0",
        "10",
        "20",
        "30",
        "40",
        "50"
    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
        {
            "from": 0,
            "to": 35,
            "color": "rgba(0, 70, 20, 140)"
        },
        {
            "from": 35,
            "to": 50,
            "color": "rgba(150, 0, 0, 140)"
        }
    ],
    colorPlate: "#222",
    colorMajorTicks: "#f5f5f5",
    colorMinorTicks: "#ddd",
    colorTitle: "#fff",
    colorUnits: "#ccc",
    colorNumbers: "#eee",
    colorNeedleStart: "rgba(240, 128, 128, 1)",
    colorNeedleEnd: "rgba(240, 128, 128, 1)",
    borderShadowWidth: 0,
    borders: false,
    needleType: "arrow",
    needleWidth: 2,
    needleCircleSize: 7,
    needleCircleOuter: true,
    needleCircleInner: false,
    animation: false,
    animationDuration: 50,
    animationRule: "quad"
}).draw();

const canvas = document.getElementById('odometry');
const ctx = canvas.getContext('2d');

// Set up WebSocket connection
function setupWebSocket() {
    socket = new WebSocket("ws://" + window.location.hostname + ":81");
    
    // console logging
    socket.onopen = () => {
        console.log("Connected to WebSocket server.");
        // requestAnimationFrame(updateUI);
        // setTimeout(updateUI, 100);
    };

    // Incoming WebSocket messages
    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.speed !== undefined) {
            speed = data.speed;
            slider.value = speed;
            output.innerHTML = `Speed:${speed} cm/s`;
        }

        if (data.buggyspeed !== undefined) {

            buggyspeed = data.buggyspeed;
            gauge.value = buggyspeed;
            console.log(data.buggyspeed);
        }

        if (data.distance !== undefined) {
            document.querySelectorAll('.distance-section p')[0].textContent = `${data.distance} cm`;
            // console.log(data.distance);
        }

        if (data.travelled !== undefined) {
            document.querySelectorAll('.distance-section p')[1].textContent = `${data.travelled.toFixed(2)} m`;
        }

        if (data.TagID !== undefined) {
            id = data.TagID;
            // console.log("Tag ID:", id);
            document.querySelectorAll('.variables p')[3].textContent = `Tag ID: ${id}`;
        }

        if (data.mode !== undefined) {
            mode = data.mode;
            updateModeText(mode); // Update mode description based on mode
        }

        if (data.obstacle !== undefined) {
            obstacleDetected = data.obstacle;
            updateObstacleAlert(obstacleDetected);
            // console.log(data.obstacle);
            //toogle obstacle detection
        }

        if (data.enable !== undefined) {
            enable = data.enable;
            updateStartButton();    
            /*update start button base on message, syncs multiple clients so that they all have the same state for start and stop*/
        }

        if (data.renesas !== undefined) {
            // console.log("Renesas Loop Time:", data.renesas);
            document.querySelectorAll('.variables p')[0].textContent = `Renesas Loop Time: ${data.renesas}`;
        }

        if (data.esp !== undefined) {
            // console.log("ESP32 Loop Time:", data.esp);
            document.querySelectorAll('.variables p')[1].textContent = `ESP32 Loop Time: ${data.esp}`;
        }

        if (data.peak !== undefined) {
            // console.log("Peak:", data.peak);
            document.querySelectorAll('.variables p')[2].textContent = `Peak ESP Loop Time: ${data.peak}`;
        }

        if (data.reset) {
            trackPoints = [];
            minX = Infinity;
            maxX = -Infinity;
            minY = Infinity;
            maxY = -Infinity;
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            console.log("Path reset.");
        }

        if (data.position && data.position.x !== undefined && data.position.y !== undefined) {
            x = data.position.x;
            y = data.position.y;
            addPoint(x, -y);
            console.log("Position:", x, y);
        }

        updateUI();
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

    console.log("Sending command:", !enable ? "start" : "stop");
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
    if (obstacleDetected) {
        document.body.classList.add('flash-bg');
    } else {
        document.body.classList.remove('flash-bg');
    }
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
            modeText = "Unknown Mode"   ;
            break;
    }
    modeSelect.textContent = `${modeText}`;
}

function updateSpeedValue() {
    if (!isChangingSpeedSlider) {
        slider.value = speed;
    }
}

function updateImage() {

    if (obstacleDetected) {
        imageElements.forEach(image => {
            image.src = images[3];
          });

    } else if (!enable) {
        imageElements.forEach(image => {
            image.src = "";
          });

    } else {
        imageElements.forEach(image => {
            image.src = images[id];
          });
    }
}

function addPoint(x, y) {
    trackPoints.push({ x: x, y: y });

    trackPoints.forEach(p => {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
      });

    drawPath();
}

function drawPath() {
    // Clear the canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Check if we have any points
    if (trackPoints.length === 0) return;

    const effectiveWidth = canvas.width - 2 * padding;
    const effectiveHeight = canvas.height - 2 * padding;

    const scaleX = effectiveWidth / (maxX - minX);
    const scaleY = effectiveHeight / (maxY - minY);
    
    const scale = Math.min(scaleX, scaleY);
    
    const translateX = padding - (minX * scale);
    const translateY = padding - (minY * scale);

    ctx.save();

    ctx.translate(translateX, translateY);
    ctx.scale(scale, scale);
    
    // Draw the path
    ctx.beginPath();
    ctx.moveTo(trackPoints[0].x, trackPoints[0].y);
    trackPoints.forEach(point => {
      ctx.lineTo(point.x, point.y);
    });
    ctx.strokeStyle = 'white';
    ctx.lineWidth = 3 / scale;
    ctx.stroke();

    ctx.restore();
}

function updateUI() {
    gauge.value = buggyspeed;
    console.log("drawing");
    updateImage();
}

// updateModeText(mode); // might be redundant but havent tested
setupWebSocket();