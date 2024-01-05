const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Micro Controller Oscilloscope</title>
	<style type="text/css">
		/**
		 * CSS Variables
		 */
		
		:root {
		  --canvasWidth: 60vw;
		  --canvasHeight: 100vh;
		  --menuWidth: 40vw;
		  --menuHeight: 100vh;
		  --menuButtonWidth: 60px;
		  --menuButtonHeigt: 60px;
		}
		
		/**
		 * Element Settings
		 */
		
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
		}
		
		/**
		 * Navigation Menu
		 */
		
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
		  width: 205px;
		  height: 25px;
		  border-radius: 15px;
		  display: flex;
		  justify-content: center;
		  align-items: center;
		  padding: 0px;
		}
		
		/**
		 * Oscilloscope Elements
		 */
		
		.oscilloscope {
		  float: left;
		  background-color: #000000;
		  width: var(--canvasWidth, 60vw);
		  height: var(--canvasHeight, 100vh);
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
		
		.channel-select {
		  /*
		  overflow-y: hidden;
		  overflow-x: scroll;
		  width: var(--menuWidth, 40vw);
		  height: var(--menuButtonHeigt, 60px);
		  display: inline-block;
		  white-space: nowrap
		  */
		}
		
		/**
		 * Active Menu Buttons
		 */
		
		.button-channel {
		  float: left;
		  border-radius: 10px;
		  border: 0;
		  padding: 3px;
		  color: white;
		  background-color: #00af00;
		  border: 5px solid #00af00;
		  width: var(--menuButtonWidth, 60px);
		  height: var(--menuButtonHeigt, 60px);
		}
		
		.button-channel:hover {
		  background-color: #2ec27e;
		}
		
		.button-channel:active {
		  background-color: green;
		}
		
		/**
		 * Inactive Menu Buttons
		 */
		
		.button-channel-disabled {
		  float: left;
		  border-radius: 10px;
		  border: 0;
		  padding: 3px;
		  color: white;
		  background-color: #c01c28;
		  border: 5px solid #c01c28;
		  width: var(--menuButtonWidth, 60px);
		  height: var(--menuButtonHeigt, 60px);
		}
		
		.button-channel-disabled:hover {
		  background-color: #ed333b;
		}
		
		.button-channel-disabled:active {
		  background-color: #800000;
		}
		
		/**
		 * Disabled Menu Buttons
		 */
		
		/**
		 * Selected Channels
		 */
		
		.selected-channel {
		  background-color: #4b1212;
		  border-radius: 20px;
		  padding: 10px;
		  border: 5px solid black;
		}
		
		.selected-channel-highlight {
		  border: 5px solid black;
		}

	</style>
</head>

<body>
	<div class="oscilloscope">
		<canvas id="canvas" class="oscilloscope-canvas" width="100" height="100"></canvas>
	</div>
	<div class="oscilloscope-menu">
		<div id="channel-select" class="channel-select">
		</div>
		<br>
		<button id="updaterer">update</button>
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
				<button class="navigation-button" id="zoom-in">+</button>
				<button class="navigation-button" id="zoom-out">-</button>
			</div>
		</div>
	</div>

	<script>
		let canvas = document.getElementById("canvas");
		let drawWorker = new makeWorker(`
			let canvas = new OffscreenCanvas(20, 20);
			let ctx;
			let screenCanvas;
			let screenCTX;
			
			//Stores oscilloscope channel data
			const backColor = "#000000";
			const gridColor = "#808080";
			const foreColor = "#FFFFFF";
			const originColor = "#FF0000";
			const textStyle = "12px serif";
			
			//////////////// Canvas Data ////////////////
			let canvasWidth;
			let canvasHeight;
			let numChannels = 0;
			let maxFrequency;
			let adcBits;
			let adcMax;
			let maxVoltage;
			let colors = ["#FFFF00", "#00FF00", "#0000FF", "#FF0000", "#FF00FF",
			"#00FFFF", "#808000", "#800080", "#008080", "#FF8000"];
			let channels = [];
			let xZoom = 1;
			let yZoom = 1;
			let oscX;
			let oscY;
			let channelPadding = 100;
			let leftPadding = 100;
			let rightPadding = 10 + channelPadding;
			let topPadding = 15;
			let bottomPadding = 35;
			let prevHorizontal = true;
			let rightLegend = true;
			let centerX;
			let centerY;
			//Should be a power of 2
			let lineSpacing = 64;
			/////////////////////////////////////////////
			
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
			
			self.onmessage = function(e) {
			    
			    const {canvas: canvasMessage} = e.data;
			    if (canvasMessage) {
			        screenCanvas = canvasMessage;
			        ctx = canvas.getContext("2d");
			        screenCTX = screenCanvas.getContext("2d");
			    }
			    else if (Object.hasOwn(e.data, 'numChannels')) {
			        numChannels = e.data.numChannels;
			        maxFrequency = e.data.maxFrequency;
			        adcBits = e.data.adcBits;
			        adcMax = e.data.adcMax;
			        maxVoltage = e.data.maxVoltage;
			        colors = e.data.colors;
			    }
			    else if (Object.hasOwn(e.data, 'canvasWidth')) {
			        canvas.width = e.data.canvasWidth;
			        canvas.height = e.data.canvasHeight;
			        screenCanvas.width = e.data.canvasWidth;
			        screenCanvas.height = e.data.canvasHeight;
			        centerX = e.data.centerX;
			        centerY = e.data.centerY;
			    }
			    else if (Object.hasOwn(e.data, 'xZoom')) {
			        xZoom = e.data.xZoom;
			        yZoom = e.data.yZoom;
			        oscX = e.data.oscX;
			        oscY = e.data.oscY;
			        lineSpacing = e.data.lineSpacing;
			    }
			    else if (Object.hasOwn(e.data, 'channelPadding')) {
			        channelPadding = e.data.channelPadding;
			        leftPadding = e.data.leftPadding;
			        rightPadding = e.data.rightPadding;
			        topPadding = e.data.topPadding;
			        bottomPadding = e.data.bottomPadding;
			        prevHorizontal = e.data.prevHorizontal;
			        rightLegend = e.data.rightLegend;
			    }
			    else if (Object.hasOwn(e.data, 'channels')) {
			
			        channels = e.data.channels;
			        ctx.lineWidth = 2;
			        ctx.font = textStyle;
			
			        ctx.clearRect(0, 0, canvas.width, canvas.height);
			        drawGrid(false);
			        ctx.setLineDash([]);
			
			        if (xZoom < 32 && xZoom >= 1) {
			            ctx.lineWidth = 1;
			        }
			        
			        for (let j = 0; j < numChannels; j++) {
			            
			            if (channels[j].enabled) {
			                setCanvas(colors[j], colors[j]);
			                ctx.beginPath();
			                //Draws graph
			                if (xZoom < 1) {
			                    for (let i = 0; i < canvas.width - rightPadding; i++) {
			                        let max = 100;
			                        let min = 300;
			                        ctx.moveTo(oscX + i, oscY - min);
			                        ctx.lineTo(oscX + i,  oscY - max);
			                    }
			                }
			                else {
			                    ctx.moveTo(oscX, oscY - channels[j].readings[0] * yZoom);
			                    for (let i = 0; i < channels[j].readings.length; i++) {
			                        ctx.lineTo(oscX + i * xZoom, oscY - channels[j].readings[i] * yZoom);
			                    }
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
			
			        screenCTX.clearRect(0, 0, canvas.width, canvas.height);
			        screenCTX.drawImage(canvas, 0, 0);
			        self.postMessage("update");
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
		`);
		let dataWorker = new makeWorker(`
			const maxReadings = 1000;
			let numChannels = 0;
			let adcMax = 0;
			
			self.onmessage = function(e) {
			    if (Object.hasOwn(e.data, 'numChannels')) {
			        numChannels = e.data.numChannels;
			        adcMax = e.data.adcMax;
			    }
			    else if (Object.hasOwn(e.data, 'channels') && numChannels > 0) {
			        for (let i = 0; i < numChannels; i++) {
			            if (e.data.channels[i].readings.length >= maxReadings) {
			                e.data.channels[i].readings.splice(0, 1);
			            }
			            e.data.channels[i].readings[e.data.channels[i].readings.length] = Math.round(Math.random() * adcMax);
			        }
			        //self.postMessage(e.data.channels);
			        self.postMessage(e.data);
			    }
			}
		`);
		let root = document.querySelector(':root');
		//let rootStyle = getComputedStyle(root);
		const canvasPercent = 60;
		//Oscilloscope variables
		let selectedChannel = -1;
		let verticalInvert = false, horizontalInvert = false;
		let xAdjust = 5, yAdjust = 5;
		let mouseX = 0, mouseY = 0;
		
		/**
		 * Interval Functions
		 */
		
		//Polls data
		setInterval(function() {
		    dataWorker.postMessage(updatedValues);
		}, 20);
		
		//Polls key presses
		setInterval(function() {
		    adjustGrid();
		}, 20);
		
		/**
		 * Custom Objects
		 */
		
		//Stores individual channel data
		function Channel(id) {
		    this.channelID = id;
		    this.enabled = true;
		    this.selected = false;
		    this.readings = [];
		    this.upperResistance = 0;
		    this.lowerResistance = 0;
		};
		
		//Creates a webworker in same js file
		//Credit to https://briangrinstead.com/blog/load-web-workers-without-a-javascript-file/
		function makeWorker(script) {
		    let URL = window.URL || window.webkitURL;
		    let Blob = window.Blob;
		    let Worker = window.Worker;
		
		    if (!URL || !Blob || !Worker || !script) {
		        return null;
		    }
		
		    let blob = new Blob([script]);
		    let worker = new Worker(URL.createObjectURL(blob));
		    return worker;
		}
		
		//Stores initiating values
		function InitValues() {
		    this.numChannels = 0;
		    this.maxFrequency;
		    this.adcBits;
		    this.adcMax;
		    this.maxVoltage;
		    this.colors = ["#FFFF00", "#00FF00", "#0000FF", "#FF0000", "#FF00FF",
		    "#00FFFF", "#808000", "#800080", "#008080", "#FF8000"];
		}
		
		//Stores canvas 
		function CanvasSize() {
		    this.canvasWidth;
		    this.canvasHeight;
		    this.centerX;
		    this.centerY;
		}
		
		function UpdatedValues() {
		    this.channels = [];
		}
		
		function CanvasMovement() {
		    this.xZoom = 1;
		    this.yZoom = 1;
		    this.oscX;
		    this.oscY;
		    //Should be a power of 2
		    this.lineSpacing = 64;
		}
		
		function CanvasPadding() {
		    this.channelPadding = 100;
		    this.leftPadding = 100;
		    this.rightPadding = 10 + this.channelPadding;
		    this.topPadding = 15;
		    this.bottomPadding = 35;
		    this.prevHorizontal = true;
		    this.rightLegend = true;
		}
		
		let initValues = new InitValues();
		let canvasSize = new CanvasSize();
		let updatedValues = new UpdatedValues();
		let canvasMovement = new CanvasMovement();
		let canvasPadding = new CanvasPadding();
		
		init();
		function init() {
		    //////////Requested from microcontroller//////////
		    initValues.numChannels = 12;
		    initValues.maxFrequency = 80000000;
		    initValues.adcBits = 10;
		    initValues.adcMax = 2 ** initValues.adcBits - 1;
		    initValues.maxVoltage = 5;
		    drawWorker.postMessage(initValues);
		
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
		    let buttons = "<button class='button-channel' style='font-size: 40px'>&#9881;</button>";
		    //Adds button channels
		    for (let i = 0; i < initValues.numChannels; i++) {
		        if (i >= initValues.colors.length) {
		            let value = Math.round(Math.random() * (16 ** 6 - 1));
		            initValues.colors[i] = "#" + value.toString(16).toUpperCase();
		            pins[i] = "A" + i.toString();
		        }
		
		        updatedValues.channels[i] = new Channel(i);
		        buttons += "<button id='channel" + i +
		        "' class='button-channel'>Ch: " + i.toString() + "<br>Pin: " + pins[i] + "</button>";
		    }
		    document.getElementById("channel-select").innerHTML = buttons;
		    drawWorker.onmessage = function(e) {
		        drawWorker.postMessage(updatedValues);
		    };
		    dataWorker.onmessage = function(e) {
		        updatedValues.channels = e.data.channels;
		    };
		    canvas = document.getElementById("canvas");
		    canvas = canvas.transferControlToOffscreen();
		    
		    drawWorker.postMessage({canvas}, [canvas]);
		    updateScreenSize();
		    dataWorker.postMessage(initValues);
		    drawWorker.postMessage(initValues);
		    drawWorker.postMessage(canvasSize);
		    drawWorker.postMessage(updatedValues);
		    drawWorker.postMessage(canvasMovement);
		}
		
		function updateScreenSize() {
		    //Gets current screen size
		    const width = window.innerWidth || document.documentElement.clientWidth ||
		        document.body.clientWidth;
		    const height = window.innerHeight || document.documentElement.clientHeight ||
		        document.body.clientHeight;
		        
		  
		    if (height > width) {
		        setCSS(100, canvasPercent, 100, 100 - canvasPercent);
		        //Adjusts size from pixel ratio
		        canvasSize.canvasWidth = width * window.devicePixelRatio;
		        canvasSize.canvasHeight = height * canvasPercent / 100 * window.devicePixelRatio;
		    }
		    else {
		        setCSS(canvasPercent, 100, 100 - canvasPercent, 100);
		        //Adjusts size from pixel ratio
		        canvasSize.canvasWidth = width * canvasPercent / 100 * window.devicePixelRatio;
		        canvasSize.canvasHeight = height * window.devicePixelRatio;
		    }
		    
		    if ((canvasSize.canvasWidth - canvasPadding.leftPadding - canvasPadding.rightPadding - canvasPadding.channelPadding) >=
		            (canvasSize.canvasHeight - canvasPadding.topPadding - canvasPadding.bottomPadding) && !canvasPadding.prevHorizontal) {
		        canvasPadding.rightLegend = true;
		        canvasPadding.rightPadding += canvasPadding.channelPadding;
		        canvasPadding.bottomPadding -= canvasPadding.channelPadding;
		        canvasPadding.prevHorizontal = true;
		    }
		    else if ((canvasSize.canvasWidth - canvasPadding.leftPadding - canvasPadding.rightPadding) <
		            (canvasSize.canvasHeight - canvasPadding.topPadding - canvasPadding.bottomPadding - canvasPadding.channelPadding) && canvasPadding.prevHorizontal) {
		        canvasPadding.rightLegend = false;
		        canvasPadding.rightPadding -= canvasPadding.channelPadding;
		        canvasPadding.bottomPadding += canvasPadding.channelPadding;
		        canvasPadding.prevHorizontal = false;
		    }
		
		    canvasSize.centerX = (canvasSize.canvasWidth - canvasPadding.leftPadding - canvasPadding.rightPadding) / 2 + canvasPadding.leftPadding;
		    canvasSize.centerY = (canvasSize.canvasHeight - canvasPadding.topPadding - canvasPadding.bottomPadding) / 2 + canvasPadding.topPadding;
		
		    if (canvasMovement.oscX === undefined) {
		        canvasMovement.oscX = (canvasSize.canvasWidth - canvasPadding.leftPadding - canvasPadding.rightPadding) / 2 + canvasPadding.leftPadding;
		        canvasMovement.oscY = (canvasSize.canvasHeight - canvasPadding.topPadding - canvasPadding.bottomPadding) / 2 + canvasPadding.topPadding;
		    }
		    drawWorker.postMessage(canvasPadding);
		}
		  
		function setCSS(canvasWidth, canvasHeight, menuWidth, menuHeight) {
		    root.style.setProperty('--canvasWidth', canvasWidth.toString() + 'vw');
		    root.style.setProperty('--canvasHeight', canvasHeight.toString() + 'vh');
		    root.style.setProperty('--menuWidth', menuWidth.toString() + 'vw');
		    root.style.setProperty('--menuHeight', menuHeight.toString() + 'vh');
		}
		
		/**
		 * Channel Functions
		 */
		
		//Sets channels according to 
		function setChannels() {
		    for (let i = 0; i < initValues.numChannels; i++) {
		        if (updatedValues.channels[i].enabled) {
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
		        if (updatedValues.channels[code].enabled) {
		            updatedValues.channels[code].enabled = false;
		        }
		        else {
		            updatedValues.channels[code].enabled = true;
		        }
		    }
		    else if (selectedChannel == -1) {
		        selectedChannel = code;
		        updatedValues.channels[selectedChannel].selected = true;
		        document.getElementById("channel" + selectedChannel.toString()).classList.add('selected-channel-highlight');
		    }
		    else {
		        updatedValues.channels[selectedChannel].selected = false;
		        document.getElementById("channel" + selectedChannel.toString()).classList.remove('selected-channel-highlight');
		        selectedChannel = code;
		        updatedValues.channels[selectedChannel].selected = true;
		        document.getElementById("channel" + selectedChannel.toString()).classList.add('selected-channel-highlight');
		    }
		    setChannels();
		}
		  
		//Adds event listeners to buttons
		addButtonEvents();
		function addButtonEvents() {
		    const multiplier = 5;
		    document.getElementById("updaterer").addEventListener("click", function() {
		        //draw();
		    });
		    for (let i = 0; i < initValues.numChannels; i++) {
		        document.getElementById("channel" + i.toString()).addEventListener("click", function() {
		            changeChannel(i);
		        });
		    }
		    document.getElementById("expand-width").addEventListener("click", function() {
		        canvasMovement.xZoom *= 2;
		    });
		    document.getElementById("expand-height").addEventListener("click", function() {
		        canvasMovement.yZoom *= 2;
		    });
		    document.getElementById("compress-width").addEventListener("click", function() {
		        canvasMovement.xZoom /= 2;
		    });
		    document.getElementById("compress-height").addEventListener("click", function() {
		        canvasMovement.yZoom /= 2;
		    });
		    if (/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent)) {
		        document.getElementById("move-left").addEventListener("click", function() {
		            canvasMovement.oscX += xAdjust * multiplier;
		        });
		        document.getElementById("move-right").addEventListener("click", function() {
		            canvasMovement.oscX -= xAdjust * multiplier;
		        });
		        document.getElementById("move-up").addEventListener("click", function() {
		            canvasMovement.oscY += xAdjust * multiplier;
		        });
		        document.getElementById("move-down").addEventListener("click", function() {
		            canvasMovement.oscY -= xAdjust * multiplier;
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
		    document.getElementById("zoom-in").addEventListener("click", function() {
		        canvasMovement.lineSpacing *= 2;
		        canvasMovement.xZoom *= 2;
		        canvasMovement.yZoom *= 2;
		    });
		    document.getElementById("zoom-out").addEventListener("click", function() {
		        canvasMovement.lineSpacing /= 2;
		        canvasMovement.xZoom /= 2;
		        canvasMovement.yZoom /= 2;
		    });
		}
		
		/**
		 * Event Listeners
		 */
		
		document.getElementById("canvas").addEventListener("wheel", function(e) {
		    const multiplier = 5;
		    if (e.deltaX > 0) {
		        canvasMovement.oscX += xAdjust * multiplier;
		    }
		    else if (e.deltaX < 0) {
		        canvasMovement.oscX -= xAdjust * multiplier;
		    }
		    if (e.deltaY > 0) {
		        canvasMovement.oscY += yAdjust * multiplier;
		    }
		    else if (e.deltaY < 0) {
		        canvasMovement.oscY -= yAdjust * multiplier;
		    }
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
		        canvasMovement.oscX -= xAdjust;
		    }
		    else if (!arrowLeft && arrowRight) {
		        canvasMovement.oscX += xAdjust;
		    }
		    if (arrowUp && !arrowDown) {
		        canvasMovement.oscY -= yAdjust;
		    }
		    else if (!arrowUp && arrowDown) {
		        canvasMovement.oscY += yAdjust;
		    }
		    drawWorker.postMessage(canvasMovement);
		}
		
		addEventListener("mousemove", function(e) {
		    mouseX = e.offsetX;
		    mouseY = e.offsetY;
		});
		
		//document.getElementById("canvas").
		addEventListener("keydown", function(e) {
		    document.getElementById("data").innerHTML = e.key;
		
		    if (mouseX <= canvasSize.canvasWidth && mouseY <= canvasSize.canvasHeight) {
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
		            canvasMovement.xZoom *= 2;
		        }
		        else if (code == ">") {
		            canvasMovement.xZoom /= 2;
		        }
		        else if (code == "^") {
		            canvasMovement.yZoom *= 2;
		        }
		        else if (code == "_") {
		            canvasMovement.yZoom /= 2;
		        }
		        else if (code == "+") {
		            canvasMovement.lineSpacing *= 2;
		            canvasMovement.xZoom *= 2;
		            canvasMovement.yZoom *= 2;
		        }
		        else if (code == "-") {
		            canvasMovement.lineSpacing /= 2;
		            canvasMovement.xZoom /= 2;
		            canvasMovement.yZoom /= 2;
		        }
		    }
		    
		    let units = "";
		
		    units += initValues.maxVoltage.toString();
		    units += " / (2^" + initValues.adcBits.toString() + ") /";
		    units += canvasMovement.yZoom + " * " + lineSpacing.toString() +  " V";
		
		    document.getElementById("units").innerHTML = units;
		    
		    //drawWorker.postMessage(canvasMovement);
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
		    drawWorker.postMessage(canvasSize);
		});
	</script>
</body>

</html>)=====";