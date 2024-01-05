let canvas = document.getElementById("canvas");
let drawWorker = new Worker('draw.js');
let dataWorker = new Worker('data.js');
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