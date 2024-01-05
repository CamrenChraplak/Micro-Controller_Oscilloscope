const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <style type="text/css">
  </style>
  <script>

    function send(led, led_sts) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var message = JSON.parse(this.responseText);
          if (message.green != undefined) {
            document.getElementById("GREENLED").innerHTML = message.green;
          }
          if (message.red != undefined) {
            document.getElementById("REDLED").innerHTML = message.red;
          }
        }
      };
      xhttp.open("GET", "led_set?" + led + "="+led_sts, true);
      xhttp.send();
    }

    setInterval(function() {
      getData();
    }, 20);

    function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          //document.getElementById("adc_val").innerHTML = this.responseText;

          var message = JSON.parse(this.responseText);
          if (message.adc != undefined) {
            document.getElementById("adc_val").innerHTML = message.adc;
          }
          if (message.time != undefined) {
            document.getElementById("time").innerHTML = message.time;
          }
        }
      };
      xhttp.open("GET", "adcread", true);
      xhttp.send();
    }

  </script>
  <body>
    <center>
      <h1>AJAX Server</h1>
      <button onclick="send('red', 1)">RED ON</button>
      <button onclick="send('red', 0)">RED OFF</button><BR><BR>
      <button onclick="send('green', 1)">GREEN ON</button>
      <button onclick="send('green', 0)">GREEN OFF</button>
      <br>
      <h2>
        ADC: <span id="adc_val">0</span><br><br>
        Red LED State: <span id="REDLED">off</span><br><br>
        Green LED State: <span id="GREENLED">off</span><br><br>
        Up Time: <span id="time">Not Started</span>
      </h2>
    </center>
  </body>
</html>
)=====";

const char webpager[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Micro Controller Oscilloscope</title>
  <style type="text/css">
  :root {
  --canvasWidth: 60vw;
  --canvasHeight: 100vh;
  --menuWidth: 40vw;
  --menuHeight: 100vh;
}

body {
  margin: 0;
  background-color: #4b1212;
  color: white;
}

button {
  cursor: pointer;
}

button span {
  display: inline-block;
  transform: rotate(90deg);
}

canvas {
  width: var(--canvasWidth, 60vw);
  height: var(--canvasHeight, 100vh);
  overflow: hidden;
}

.canvas-navigation {
  position: fixed;
  bottom: 0;
  width: var(--menuWidth, 40vw);
  text-align: center;
  padding: 2px 0px;

  display: flex;
  justify-content: center;
  align-items: center;
}

.navigation-button {
  background-color: #00af00;
  border: 0;
  border-radius: 10px;
  width: 20px;
  height: 20px;
  padding: 0px;
}

.centered-elements {
  background-color: #000000;
  width: 165px;
  height: 25px;
  border-radius: 15px;
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 0px;
}

.oscilloscope {
  float: left;
  background-color: #000000;
  width: var(--canvasWidth, 60vw);
  height: var(--canvasHeight, 100vh);
}

.channel-select button {
  float: left;
}

.updater {
  color: white;
  width: 60px;
  height: 30px;
  background-color: green;
  padding: 0px;
  border: 0;
  border-radius: 10px;
}

.button-channel {
  border-radius: 10px;
  border: 0;
  padding: 3px;
  color: white;
  background-color: #00af00;
  border: 5px solid #00af00;
}

.button-channel:hover {
  background-color: #2ec27e;
}

.button-channel:active {
  background-color: green;
}

.button-channel-disabled {
  border-radius: 10px;
  border: 0;
  padding: 3px;
  color: white;
  background-color: #c01c28;
  border: 5px solid #c01c28;
}

.button-channel-disabled:hover {
  background-color: #ed333b;
}

.button-channel-disabled:active {
  background-color: #800000;
}

.button-width-adjust {
  width: 50px;
  height: 20px;
  border-radius: 10px;
  border: 0;
  padding: 0px;
  color: white;
  background-color: green;
}

.button-width-adjust:active {
  background-color: blue;
}

.selected-channel {
  background-color: #4b1212;
  border-radius: 20px;
  padding: 10px;
  border: 5px solid black;
}

.selected-channel-highlight {
  border: 5px solid black;
}

.oscilloscope-menu {
  float: left;
  overflow-x: hidden;
  overflow-y: scroll;
  width: var(--menuWidth, 40vw);
  height: var(--menuHeight, 100vh);
}

.oscilloscope-canvas {
  display: block;
  background-color: #000000;
  float: left;
  padding: 0px;
  border-bottom: 0px;
  border-width: 0px;
}
  </style>
</head>

<body>
  <div class="oscilloscope">
    <canvas id="canvas" class="oscilloscope-canvas" width="100%" height="100%" tabindex="2"></canvas>
  </div>
  <div class="oscilloscope-menu">
    <div id="channel-select" class="channel-select">
    </div>
    <div id="pose-control">
      <button id="moveLeft" class="button-width-adjust">left</button>
      <button id="moveRight" class="button-width-adjust">right</button>
    </div>
    <br>
    <button id="updaterer" class="updater">update</button>
    <div id="data">blank</div>
    test<br>
    testor
    <div id="units">units</div>
    <div class="canvas-navigation">
      <div class="centered-elements">
        <button class="navigation-button" id="move-left"><</button>
        <button class="navigation-button" id="move-right">></button>
        <button class="navigation-button" id="move-up"><span><</span></button>
        <button class="navigation-button" id="move-down"><span>></span></button>
        <button class="navigation-button" id="expand-width"><></button>
        <button class="navigation-button" id="compress-width">><</button>
        <button class="navigation-button" id="expand-height"><span><></span></button>
        <button class="navigation-button" id="compress-height"><span>><</span></button>
      </div>
    </div>
  </div>
  
  <script>
  /**
 * Global Variables
 */

//Canvas properties
let canvas = document.getElementById("canvas");
let ctx;
let root = document.querySelector(':root');
let rootStyle = getComputedStyle(root);
//const canvasPercent = rootStyle.getPropertyValue('--canvasPercent') / 100;
const canvasPercent = 60;
//Stores oscilloscope channel data
let numChannels;
let channels = [];
const colors = ["#FFFF00", "#00FF00", "#0000FF", "#FF0000", "#FF00FF",
    "#00FFFF", "#808000", "#800080", "#008080", "#FF8000"];
const backColor = "#000000";
const gridColor = "#808080";
const foreColor = "#FFFFFF";
const originColor = "#FF0000";
const textStyle = "12px serif";
//Sets padding constants
let channelPadding = 100;
let topPadding = 15;
let bottomPadding = 35;
let leftPadding = 100;
let rightPadding = 10 + channelPadding;
let prevHorizontal = true;
let rightLegend = true;
//Should be a power of 2
const lineSpacing = 128;
//Oscilloscope variables
let maxFrequency, adcMax, maxVoltage, adcBits;
let centerX, centerY;
let oscX, oscY;
let voltUnit, freqUnit;
let selectedChannel = -1;
//Zoom in relation to 1/f and Vmax/ADCmax
let xZoom = 16, yZoom = 1;
let verticalInvert = false, horizontalInvert = false;
let xAdjust = 5, yAdjust = 5;
let mouseX = 0, mouseY = 0;

/**
 * Custom Objects
 */

//Stores individual channel data
function channel(id) {
  this.channelID = id;
  this.enabled = true;
  this.selected = false;
  //Coordinates in relation to (0,0)
  //Coordinates and compression in magnitudes of frequencySpacing
  this.x = 0; this.y = 0;
  this.compressionWidth = 20;
  this.compressionHeight = 20;
  this.readings = [];
  this.frequencySpacing = 20;
  this.upperResistance = 0;
  this.lowerResistance = 0;
  this.vUnitStep = 0;
  this.fUnitStep = 0;
};

/**
 * Interval Functions
 */

//Keeps updating screen
setInterval(function() {
  draw();
}, 25);
//Keeps polling data
setInterval(function() {
  getData();
}, 2);
//Polling key presses
setInterval(function() {
  adjustGrid();
}, 20);

/**
 * Initializes Program
 */

//Initialization
init();
function init() {
  //////////Requested from microcontroller//////////
  numChannels = 10;
  maxFrequency = 80000000;
  adcBits = 10;
  adcMax = 2 ** adcBits - 1;
  maxVoltage = 5;
  let pins = ["A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9"];
  if (verticalInvert) {
    yAdjust *= -1;
  }
  if (horizontalInvert) {
    xAdjust *= -1;
  }
  //////////////////////////////////////////////////
  mouseX = window.offsetX;
  mouseY = window.offsetY;
  voltUnit = maxVoltage / (adcMax + 1);
  freqUnit = 1 / maxFrequency;
  let buttons = "";
  //Adds button channels
  for (let i = 0; i < numChannels; i++) {

    if (i >= colors.length) {
      let value = Math.round(Math.random() * (16 ** 6 - 1));
      colors[i] = "#" + value.toString(16).toUpperCase();
      pins[i] = "A" + i.toString();
    }

    channels[i] = new channel(i);
    buttons += "<button id='channel" + i +
        "' class='button-channel'>Ch: " + i.toString() + "<br>Pin: " + pins[i] + "</button>";
  }
  document.getElementById("channel-select").innerHTML = buttons;
  setChannels();
  for (let i = 0; i < 1000; i++) {
    getData();
  }
  updateScreenSize();
}

function getData() {
  const maxReadings = 1000;
  for (let i = 0; i < numChannels; i++) {
    if (channels[i].readings.length >= maxReadings) {
      channels[i].readings.splice(0, 1);
    }
    channels[i].readings[channels[i].readings.length] = Math.round(Math.random() * adcMax);
  }
}

function setCanvas(fillColor, strokeColor) {
  ctx.fillStyle = fillColor;
  ctx.strokeStyle = strokeColor;
}

function setCanvas(fillColor, strokeColor, line, gap, align) {
  ctx.fillStyle = fillColor;
  ctx.strokeStyle = strokeColor;
  ctx.setLineDash([line, gap]);
  ctx.textAlign = align;
}

function updateScreenSize() {
  //Gets current screen size
  const width = window.innerWidth || document.documentElement.clientWidth ||
      document.body.clientWidth;
  const height = window.innerHeight || document.documentElement.clientHeight ||
      document.body.clientHeight;
  //Gets canvas context
  ctx = canvas.getContext("2d");

  if (window.innerHeight > window.innerWidth) {
    setCSS(100, canvasPercent, 100, 100 - canvasPercent);
    
    //Adjusts size from pixel ratio
    canvas.width = width * window.devicePixelRatio;
    canvas.height = height * canvasPercent / 100 * window.devicePixelRatio;
    /*
    if (prevHorizontal) {
      rightLegend = false;
      rightPadding -= channelPadding;
      bottomPadding += channelPadding;
      prevHorizontal = false;
    }
    */
  }
  else {
    setCSS(canvasPercent, 100, 100 - canvasPercent, 100);
    
    //Adjusts size from pixel ratio
    canvas.width = width * canvasPercent / 100 * window.devicePixelRatio;
    canvas.height = height * window.devicePixelRatio;
    /*
    if (!prevHorizontal) {
      rightLegend = true;
      rightPadding += channelPadding;
      bottomPadding -= channelPadding;
      prevHorizontal = true;
    }
    */
  }
  if ((canvas.width - leftPadding - rightPadding - channelPadding) >=
      (canvas.height - topPadding - bottomPadding) && !prevHorizontal) {
    rightLegend = true;
    rightPadding += channelPadding;
    bottomPadding -= channelPadding;
    prevHorizontal = true;
  }
  else if ((canvas.width - leftPadding - rightPadding) <
      (canvas.height - topPadding - bottomPadding - channelPadding) && prevHorizontal) {
    rightLegend = false;
    rightPadding -= channelPadding;
    bottomPadding += channelPadding;
    prevHorizontal = false;
  }

  centerX = (canvas.width - leftPadding - rightPadding) / 2 + leftPadding;
  centerY = (canvas.height - topPadding - bottomPadding) / 2 + topPadding;

  if (oscX === undefined) {
    oscX = (canvas.width - leftPadding - rightPadding) / 2 + leftPadding;
    oscY = (canvas.height - topPadding - bottomPadding) / 2 + topPadding;
  }

  ctx.lineWidth = 2;
  ctx.font = textStyle;

  draw();
}

function setCSS(canvasWidth, canvasHeight, menuWidth, menuHeight) {
  root.style.setProperty('--canvasWidth', canvasWidth.toString() + 'vw');
  root.style.setProperty('--canvasHeight', canvasHeight.toString() + 'vh');
  root.style.setProperty('--menuWidth', menuWidth.toString() + 'vw');
  root.style.setProperty('--menuHeight', menuHeight.toString() + 'vh');
}

/**
 * Draws to Screen
 */

function drawGrid(legend) {
  ctx.textAlign = "center";
  let start = oscX % lineSpacing;
  while (start < leftPadding) start += lineSpacing;
  
  for (let cord = start; cord <= canvas.width - rightPadding; cord += lineSpacing) {
    drawGridSegment(legend, cord, false, cord, canvas.height - bottomPadding + 14,
        cord, topPadding, cord, canvas.height - bottomPadding);
  }

  start = oscY % lineSpacing;
  while (start < topPadding) start += lineSpacing;
  
  for (let cord = start; cord <= canvas.height - bottomPadding; cord += lineSpacing) {
    drawGridSegment(legend, cord, true, leftPadding / 2, cord - 2,
        leftPadding, cord, canvas.width - rightPadding, cord);
  }
}

function drawGridSegment(legend, cord, horizontal, textX, textY, startX, startY, endX, endY) {
  if (legend) {
    setCanvas(foreColor, foreColor);
    drawValues(horizontal, cord, textX, textY);
  }
  else {
    if ((cord == oscX && !horizontal) || (cord == oscY && horizontal)) {
      setCanvas("red", "red");
      ctx.setLineDash([]);
    }
    else {
      setCanvas(gridColor, gridColor);
      ctx.setLineDash([8, 8]);
    }

    ctx.beginPath();
    ctx.moveTo(startX, startY);
    ctx.lineTo(endX, endY);
    ctx.stroke();
  }
}

function drawValues(horizontal, cord, textX, textY) {
  let power = 0;
  let step;
  let unit;
  if (horizontal) {
    step = maxVoltage / (adcMax + 1);

    for (let i = cord; i < oscY; i += lineSpacing) {
      power++;
    }
    for (let i = cord; i > oscY; i -= lineSpacing) {
      power--;
    }

    unit = "V";
    if (Math.abs(step * power / yZoom * lineSpacing) < 0.1 && power != 0) {
      unit = "mV";
      step *= 1000;
    }
    if (Math.abs(step * power / yZoom * lineSpacing) < 0.1 && power != 0) {
      unit = "uV";
      step *= 1000;
    }

    ctx.fillText(parseFloat((power * step / yZoom * lineSpacing).toFixed(3)).toString() + " " + unit, textX, textY);
    ctx.fillText(power.toString() + " * " + maxVoltage.toString() + " / (2^" + adcBits.toString() + ") /", textX, textY + 22);
    ctx.fillText(yZoom + " * " + lineSpacing.toString() +  " V", textX, textY + 34);
  }
  else {
    unit = "Hz";
    power = 2;
    step = 3;

    ctx.fillText(parseFloat((power * step / yZoom * lineSpacing).toFixed(3)).toString() + " " + unit, textX, textY);
    ctx.fillText("2^" + power.toString() + " V", textX, textY + 12);
  }
  
}

function drawBounds() {
  ctx.setLineDash([]);
  setCanvas(foreColor, foreColor);
  //Draws bounds
  ctx.beginPath();
  ctx.moveTo(leftPadding, topPadding);
  ctx.lineTo(canvas.width - rightPadding, topPadding);
  ctx.lineTo(canvas.width - rightPadding, canvas.height - bottomPadding);
  ctx.lineTo(leftPadding, canvas.height - bottomPadding);
  ctx.lineTo(leftPadding, topPadding);
  ctx.lineTo(canvas.width - rightPadding, topPadding);
  ctx.stroke();
}

function drawLegend() {
  ctx.textAlign = "left";
  ctx.setLineDash([]);
  let channelCount = 0;
  for (let i = 0; i < numChannels; i++) {
    if (channels[i].enabled) {
      if (rightLegend) {
        ctx.beginPath();
        setCanvas(colors[i], colors[i]);
        ctx.moveTo(canvas.width - rightPadding + 5, topPadding + 4 + 20 * channelCount);
        ctx.lineTo(canvas.width - rightPadding + 25, topPadding + 4 + 20 * channelCount);
        ctx.stroke();
        ctx.fillStyle = foreColor;
        ctx.fillText("Channel: " + i.toString(), canvas.width - rightPadding + 30,
            topPadding + 8 + 20 * channelCount);
      }
      else {
        ctx.beginPath();
        setCanvas(colors[i], colors[i]);
        ctx.moveTo(leftPadding + 5 + 20 * channelCount, canvas.height - channelPadding + 0);
        ctx.lineTo(leftPadding + 5 + 20 * channelCount, canvas.height - channelPadding + 20);
        ctx.stroke();
        const textX = leftPadding + 0 + 20 * channelCount;
        const textY = canvas.height - channelPadding + 22;
        ctx.translate(textX, textY);
        ctx.rotate(Math.PI/2);
        ctx.fillStyle = foreColor;
        ctx.fillText("Channel: " + i.toString(), 0,
            0);
        //ctx.fillText("Channel: " + i.toString(), 0, 0);
        ctx.rotate(-Math.PI/2);
        
        ctx.translate(-textX, -textY);
      }
      channelCount++;
    }
  }
}

//Draws shapes
function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  drawGrid(false);
  ctx.setLineDash([]);
  if (xZoom < 32) {
    ctx.lineWidth = 1;
  }
  for (let j = 0; j < numChannels; j++) {
    if (channels[j].enabled) {
      setCanvas(colors[j], colors[j]);
      ctx.beginPath();
      //Draws graph
      ctx.moveTo(oscX, oscY - channels[j].readings[0] * yZoom);
      const max = canvas.width - rightPadding + lineSpacing;
      for (let i = 0; oscX + i * xZoom <= max; i++) {
        ctx.lineTo(oscX + i * xZoom, oscY - channels[j].readings[i] * yZoom);
      }
      ctx.stroke();
    }
  }
  ctx.lineWidth = 2;
  ctx.clearRect(0, 0, canvas.width, topPadding);
  ctx.clearRect(canvas.width - rightPadding, 0, rightPadding, canvas.height);
  ctx.clearRect(0, 0, leftPadding, canvas.height);
  ctx.clearRect(0, canvas.height - bottomPadding, canvas.width, canvas.height);
  drawGrid(true);
  drawLegend();
  drawBounds();
}

/**
 * Channel Functions
 */

//Sets channels according to 
function setChannels() {
  for (let i = 0; i < numChannels; i++) {
    if (channels[i].enabled) {
      document.getElementById("channel" + i.toString()).classList.remove('button-channel-disabled');
      document.getElementById("channel" + i.toString()).classList.add('button-channel');
    }
    else {
      document.getElementById("channel" + i.toString()).classList.add('button-channel-disabled');
      document.getElementById("channel" + i.toString()).classList.remove('button-channel');
    }
  }
}

//Inverts channel of button inputted
function changeChannel(code) {
  //selected-channel
  if (selectedChannel == code) {
    if (channels[code].enabled) {
      channels[code].enabled = false;
    }
    else {
      channels[code].enabled = true;
    }
  }
  else if (selectedChannel == -1) {
    selectedChannel = code;
    channels[selectedChannel].selected = true;
    document.getElementById("channel" + selectedChannel.toString()).classList.add('selected-channel-highlight');
  }
  else {
    channels[selectedChannel].selected = false;
    document.getElementById("channel" + selectedChannel.toString()).classList.remove('selected-channel-highlight');
    selectedChannel = code;
    channels[selectedChannel].selected = true;
    document.getElementById("channel" + selectedChannel.toString()).classList.add('selected-channel-highlight');
  }
  setChannels();
}

//Adds event listeners to buttons
addButtonEvents();
function addButtonEvents() {
  const multiplier = 5;
  document.getElementById("updaterer").addEventListener("click", function() {
    draw();
  });
  for (let i = 0; i < numChannels; i++) {
    document.getElementById("channel" + i.toString()).addEventListener("click", function() {
      changeChannel(i);
    });
  }
  document.getElementById("expand-width").addEventListener("click", function() {
    xZoom *= 2;
  });
  document.getElementById("expand-height").addEventListener("click", function() {
    yZoom *= 2;
  });
  document.getElementById("compress-width").addEventListener("click", function() {
    xZoom /= 2;
  });
  document.getElementById("compress-height").addEventListener("click", function() {
    yZoom /= 2;
  });
  if (/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent)) {
    document.getElementById("move-left").addEventListener("click", function() {
      oscX += xAdjust * multiplier;
    });
    document.getElementById("move-right").addEventListener("click", function() {
      oscX -= xAdjust * multiplier;
    });
    document.getElementById("move-up").addEventListener("click", function() {
      oscY += xAdjust * multiplier;
    });
    document.getElementById("move-down").addEventListener("click", function() {
      oscY -= xAdjust * multiplier;
    });
  }
  else {
    document.getElementById("move-left").addEventListener("mousedown", function() {
      arrowLeft = true;
    });
    document.getElementById("move-left").addEventListener("mouseup", function() {
      arrowLeft = false;
    });
    document.getElementById("move-right").addEventListener("mousedown", function() {
      arrowRight = true;
    });
    document.getElementById("move-right").addEventListener("mouseup", function() {
      arrowRight = false;
    });
    document.getElementById("move-up").addEventListener("mousedown", function() {
      arrowUp = true;
    });
    document.getElementById("move-up").addEventListener("mouseup", function() {
      arrowUp = false;
    });
    document.getElementById("move-down").addEventListener("mousedown", function() {
      arrowDown = true;
    });
    document.getElementById("move-down").addEventListener("mouseup", function() {
      arrowDown = false;
    });
  }

  
}

/**
 * Event Listeners
 */

document.getElementById("canvas").
addEventListener("wheel", function(e) {
  const multiplier = 5;
  if (e.deltaX > 0) {
    oscX += xAdjust * multiplier;
  }
  else if (e.deltaX < 0) {
    oscX -= xAdjust * multiplier;
  }
  if (e.deltaY > 0) {
    oscY += yAdjust * multiplier;
  }
  else if (e.deltaY < 0) {
    oscY -= yAdjust * multiplier;
  }
  draw();
});

function mouseClick(e) {
  /*
   * Button 0 is left click
   * Button 2 is right click
  */
  for (let i = 0; i < 5; i++) {
    if (e.button == i) {
      //document.getElementById("data").innerHTML = "button: " + i.toString();
    }
  }
}
/*
document.getElementById("canvas").
addEventListener("click", function(e) {
  e.preventDefault();
  mouseClick(e);
  
});
*/
/*
//document.getElementById("canvas").
addEventListener("contextmenu", function(e) {
  e.preventDefault();
  mouseClick(e);
});
*/
let arrowUp = false;
let arrowDown = false;
let arrowLeft = false;
let arrowRight = false;

function adjustGrid() {
  if (arrowLeft && !arrowRight) {
    oscX -= xAdjust;
  }
  else if (!arrowLeft && arrowRight) {
    oscX += xAdjust;
  }
  if (arrowUp && !arrowDown) {
    oscY -= yAdjust;
  }
  else if (!arrowUp && arrowDown) {
    oscY += yAdjust;
  }
}

addEventListener("mousemove", function(e) {
  mouseX = e.offsetX;
  mouseY = e.offsetY;
});

//document.getElementById("canvas").
addEventListener("keydown", function(e) {
  document.getElementById("data").innerHTML = e.key;

  if (mouseX <= canvas.width && mouseY <= canvas.height) {
    let code = e.key;
    if (code == "ArrowUp") {
      arrowUp = true;
    }
    else if (code == "ArrowDown") {
      arrowDown = true;
    }
    else if (code == "ArrowRight") {
      arrowRight = true;
    }
    else if (code == "ArrowLeft") {
      arrowLeft = true;
    }
    else if (code == "<") {
      xZoom *= 2;
      //oscX += (oscX - centerX) * 2;
    }
    else if (code == ">") {
      xZoom /= 2;
      //oscX -= (oscX - centerX) / 2;
    }
    else if (code == "+") {
      yZoom *= 2;
    }
    else if (code == "-") {
      yZoom /= 2;
    }
  }

  
  let units = "";

  //ctx.fillText(power.toString() + " * " + maxVoltage.toString() + " / (2^" + adcBits.toString() + ") /", textX, textY + 22);
  //ctx.fillText(yZoom + " * " + lineSpacing.toString() +  " V", textX, textY + 34);

  units += power.toString() + " * " + maxVoltage.toString() + " / (2^" + adcBits.toString() + ") /";
  units += yZoom + " * " + lineSpacing.toString() +  " V";

  document.getElementById("units").innerHTML = units;
});

//document.getElementById("canvas").
addEventListener("keyup", function(e) {
  document.getElementById("data").innerHTML = e.key;
  let code = e.key;
  if (code == "ArrowUp") {
    arrowUp = false;
  }
  else if (code == "ArrowDown") {
    arrowDown = false;
  }
  else if (code == "ArrowRight") {
    arrowRight = false;
  }
  else if (code == "ArrowLeft") {
    arrowLeft = false;
  }
});

window.addEventListener("resize", function() {
  updateScreenSize();
});






</script>
</body>

</html>
)=====";