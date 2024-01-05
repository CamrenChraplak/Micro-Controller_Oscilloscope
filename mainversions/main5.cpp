#include <WiFi.h>
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include "index.h"
#include "wifi_credentials.h"

#define REDLED 33
#define GREENLED 32
#define ADC 34

int redState = LOW;
int greenState = LOW;
int adcVal = 0;
int clients = 0;

WebServer server(80);
AsyncWebSocket ws("/ws");
hw_timer_t *Timer0_Cfg = NULL;

void IRAM_ATTR Timer0_ISR() {

}

String jsonText(String topic[], String content[], int size) {
    String data = "{";
    for (int i = 0; i < size; i++) {
        if (i != 0) {
            data += ",";
        }
        data += "\"" + (String)topic[i] + "\":";
        data += "\"" + (String)content[i] + "\"";
    }
    data += "}";
    return data;
}

void freeJSON(String *topic, String *content) {
    free(topic);
    free(content);
}

void notifyClients() {
	String *topic = (String*)calloc(4, sizeof(String));
    String *content = (String*)calloc(4, sizeof(String));
	topic[0] = "adc";
    topic[1] = "time";
	topic[2] = "red";
    topic[3] = "green";
    content[0] = (String)adcVal;
    content[1] = (String)millis();
	content[2] = (String)redState;
    content[3] = (String)greenState;
	ws.textAll(jsonText(topic, content, 4));
	freeJSON(topic, content);
}

void handleMessage(char* data) {
	if (strcmp(data, "r0g0") == 0) {
		redState = LOW;
		greenState = LOW;
	}
	else if (strcmp(data, "r1g0") == 0) {
		redState = HIGH;
		greenState = LOW;
	}
	else if (strcmp(data, "r0g1") == 0) {
		redState = LOW;
		greenState = HIGH;
	}
	else if (strcmp(data, "r1g1") == 0) {
		redState = HIGH;
		greenState = HIGH;
	}
	
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
		data[len] = 0;
		handleMessage((char*)data);
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
        void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
			clients++;
		break;
		case WS_EVT_DISCONNECT:
			Serial.printf("WebSocket client #%u disconnected\n", client->id());
			clients--;
		break;
		case WS_EVT_DATA:
			handleWebSocketMessage(arg, data, len);
		break;
		case WS_EVT_PONG:
		case WS_EVT_ERROR:
		break;
	}
}

String initializer(const String& var){
	return "initer value";
}

TaskHandle_t Task1;
TaskHandle_t Task2;

void timerRunner(void * pvParameters) {
    Serial.print("timerRunner running on core ");
    Serial.println(xPortGetCoreID());

    /*****
     * T[out] = TimerTicks * Prescaler / APB_CLK[80 MHz]
     * Prescaler = 1 or 2
     *    T[out] = TimerTicks * 2 / APB_CLK[80 MHz]
     * Prescaler = 0
     *    T[out] = TimerTicks * 2 / 65536[80 MHz]
     *****/
	/*
    Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 1000, true);
    timerAlarmEnable(Timer0_Cfg);
	*/

    for(;;) {
        delay(1);
    }
}

void serverRunner(void * pvParameters) {

	Serial.print("serverRunner running on core ");
    Serial.println(xPortGetCoreID());

	pinMode(REDLED, OUTPUT);
    pinMode(GREENLED, OUTPUT);

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP Local IP Address
	Serial.println(WiFi.localIP());

	ws.onEvent(onEvent);
	server.addHandler(&ws);

	// Route for root / web page
	server.on("/", HTTP_GET, [](WebServer *request){
		request->send_P(200, "text/html", webpage, initializer);
	});

	// Start server
	server.begin();

    for(;;) {
        ws.cleanupClients();
		digitalWrite(REDLED, redState);
		digitalWrite(GREENLED, greenState);
		adcVal = analogRead(ADC);
		if (WiFi.status() == WL_CONNECTED && clients > 0) {
			notifyClients();
			delay(20);
		}
    }
}

void setup() {
	// Serial port for debugging purposes
	Serial.begin(115200);

	xTaskCreatePinnedToCore (
        timerRunner,      //Function name
        "Cored0",   //Identifier
        10000,      //Stack size
        NULL,       //Input parameters
        1,          //Priority
        &Task1,     //Task handle
        0);         //Core assignment

    xTaskCreatePinnedToCore (
        serverRunner,   //Function name
        "Cored1",       //Identifier
        10000,          //Stack size
        NULL,           //Input parameters
        1,              //Priority
        &Task2,         //Task handle
        1);             //Core assignment
}

void loop() {}