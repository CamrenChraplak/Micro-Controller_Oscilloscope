#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include "wifi_credentials.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <soc/sens_reg.h>
#include <soc/sens_struct.h>

#define REDLED 33
#define GREENLED 32
#define RESISTOR 34
#define ADC 35

//T[out] = TimerTicks * Prescaler / APB_CLK[80 MHz]
#define PRESCALER 20
#define TICKS 10

int redState = LOW;
int greenState = LOW;
int adcVal = 0;
int clients = 0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
hw_timer_t *Timer0_Cfg = NULL;

int inVolts = 0;
int readIndex = 0;
int readings[10000];
//uint16_t channels[10][100][10000];
struct Channel {
	uint16_t readings[100][10000];
};
Channel channels[10];

int IRAM_ATTR local_adc1_read(int channel) {
    uint16_t adc_value = -1;
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel); // only one channel is selected
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas_start1.meas1_start_sar = 0;
    SENS.sar_meas_start1.meas1_start_sar = 1;
    while (SENS.sar_meas_start1.meas1_done_sar == 0);
    adc_value = SENS.sar_meas_start1.meas1_data_sar;
    return adc_value;
}

int IRAM_ATTR ADC1Read(int channel) {
	
}

void IRAM_ATTR Timer0_ISR() {
	//if (readIndex < 10000) {
		//readings[readIndex] = adc1_get_raw(ADC1_CHANNEL_7);
		//readings[readIndex] = analogRead(ADC);
		//readings[readIndex] = local_adc1_read(ADC1_CHANNEL_7);
		/*
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		readings[readIndex] = analogRead(ADC);
		*/
	//}
	readIndex++;
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
	
    Timer0_Cfg = timerBegin(0, PRESCALER, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, TICKS, true);
    timerAlarmEnable(Timer0_Cfg);
	
    for(;;) {
		/*
		Serial.println("");
		Serial.println(millis());
		for (uint8_t i = 0; i < 100; i++) {
			Serial.print(readings[i]);
			Serial.print(", ");
		}
		Serial.println("");
		readIndex = 0;
		*/
		//delay(1);
		//int i = 0;
    }
	
}

void serverRunner(void * pvParameters) {

	Serial.print("serverRunner running on core ");
    Serial.println(xPortGetCoreID());

	//pinMode(REDLED, OUTPUT);
    //pinMode(GREENLED, OUTPUT);
	//pinMode(RESISTOR, INPUT);
	//pinMode(ADC, INPUT);
	/*

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
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/html", webpage, initializer);
	});

	// Start server
	server.begin();

    for(;;) {
        ws.cleanupClients();
		digitalWrite(REDLED, redState);
		digitalWrite(GREENLED, greenState);
		adcVal = analogRead(RESISTOR);
		if (clients > 0) {
			notifyClients();
			delay(20);
		}
		Serial.println(inVolts);
    }
	*/
	int counter = 0;
	for(;;) {
		Serial.println("");
		Serial.println(micros());
		/*
		for (int i = 0; i < 1000; i++) {
			Serial.print(readings[i]);
			Serial.print(", ");
		}
		*/
		Serial.println(readIndex);
		Serial.println("");
		readIndex = 0;
	}
}

void setup() {
	disableCore0WDT();
	disableCore1WDT();

	//Serial.begin(115200);
	Serial.begin(1152000);
	pinMode(ADC, INPUT);
	/*
	//Sets CPU clock source as PLL_CLK (320 or 480MHz)
	RTC_CNTL_SOC_CLK_SEL = 1;

	//Sets CPU period to 2 for 240MHz for each core
	CPU_CPUPERIOD_SEL = 2;
	*/

	delay(3000);
    
	xTaskCreatePinnedToCore (
        serverRunner,		//Function name
        "Cored0",			//Identifier
        10000,				//Stack size
        NULL,				//Input parameters
        1,					//Priority
        &Task1,				//Task handle
        0);					//Core assignment
	xTaskCreatePinnedToCore (
        timerRunner,		//Function name
        "Cored1",			//Identifier
        10000,				//Stack size
        NULL,				//Input parameters
        1,					//Priority
        &Task2,				//Task handle
        1);					//Core assignment
}

void loop() {
	vTaskDelete(NULL);
}