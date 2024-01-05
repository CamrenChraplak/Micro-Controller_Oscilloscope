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