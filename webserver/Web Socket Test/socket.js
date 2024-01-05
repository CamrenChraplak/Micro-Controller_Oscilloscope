let gateway = `ws://${window.location.hostname}/ws`;
let socket;

let redState = 0;
let greenState = 0;

init();
function init() {
	initWebSocket();
	setTimeout(sendMessage, 2000);
}

function initWebSocket() {
	socket = new WebSocket(gateway);
	socket.onopen = onOpen;
	socket.onclose = function() {
		setTimeout(initWebSocket, 2000);
	};
	socket.onmessage = onMessage;
}

function onOpen(e) {}

function onMessage(e) {
	var message = JSON.parse(e.data.toString());
	document.getElementById('adc').innerHTML = message.adc;
	document.getElementById('time').innerHTML = message.time;
	document.getElementById('redLED').innerHTML = message.red;
	document.getElementById('greenLED').innerHTML = message.green;
	//socket.send("update");
	sendMessage();
}

function sendMessage() {
	//socket.send("update");
	socket.send("r" + redState.toString() + "g" + greenState.toString());
}

document.getElementById('redOn').addEventListener('click', function() {
	//socket.send('redOn');
	redState = 1;
});
document.getElementById('redOff').addEventListener('click', function() {
	//socket.send('redOff');
	redState = 0;
});
document.getElementById('greenOn').addEventListener('click', function() {
	//socket.send('greenOn');
	greenState = 1;
});
document.getElementById('greenOff').addEventListener('click', function() {
	//socket.send('greenOff');
	greenState = 0;
});