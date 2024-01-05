#include "esp32Wifi.h"

#define REDLED 33
#define GREENLED 32
#define ADC 34

WebServer server(80);
hw_timer_t *Timer0_Cfg = NULL;

unsigned long timeCount = 0;
unsigned int count = 0;

void IRAM_ATTR Timer0_ISR() {
    timeCount++;
    if (timeCount >= 1000) {
        timeCount = 0;
        count++;
    }
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

void handleRoot() {
    String s = webpage;
    server.send(200, "text/html", s);
}

void freeJSON(String *topic, String *content) {
    free(topic);
    free(content);
}

void sensorData() {

    int adc = analogRead(ADC);

    String *topic = (String*)calloc(2, sizeof(String));
    String *content = (String*)calloc(2, sizeof(String));

    topic[0] = "adc";
    topic[1] = "time";
    content[0] = (String)adc;
    content[1] = (String)millis();

    server.send(200, "text/plane", jsonText(topic, content, 2));

    freeJSON(topic, content);
}

void ledControl() {

    String state = "Error";
    String *topic = (String*)calloc(1, sizeof(String));
    String *content = (String*)calloc(1, sizeof(String));
    *topic = "Error";
    *content = "Error";
    String redState = server.arg("red");
    String greenState = server.arg("green");

    if (redState == "1") {
        digitalWrite(REDLED, HIGH); //LED ON
        state = "Red:On";
        *topic = "red";
        *content = "on";
    }
    else if (redState == "0") {
        digitalWrite(REDLED, LOW); //LED OFF
        state = "Red:Off";
        *topic = "red";
        *content = "off";
    }
    else if (greenState == "1") {
        digitalWrite(GREENLED, HIGH);
        state = "Green:On";
        *topic = "green";
        *content = "on";
    }
    else if (greenState == "0") {
        digitalWrite(GREENLED, LOW);
        state = "Green:Off";
        *topic = "green";
        *content = "off";
    }

    server.send(200, "text/plane", jsonText(topic, content, 1));

    freeJSON(topic, content);
}

TaskHandle_t Task1;
TaskHandle_t Task2;

void timerRunner(void * pvParameters) {
    Serial.print("Task0 running on core ");
    Serial.println(xPortGetCoreID());

    /*****
     * T[out] = TimerTicks * Prescaler / APB_CLK[80 MHz]
     * Prescaler = 1 or 2
     *    T[out] = TimerTicks * 2 / APB_CLK[80 MHz]
     * Prescaler = 0
     *    T[out] = TimerTicks * 2 / 65536[80 MHz]
     *****/
    Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 1000, true);
    timerAlarmEnable(Timer0_Cfg);

    for(;;) {
        delay(1);
    }
}

void serverRunner(void * pvParameters) {

    //SPIFFS.begin();
    Serial.print("serverRunner running on core ");
    Serial.println(xPortGetCoreID());

    WiFi.begin(ssid, password);
    Serial.println("");

    pinMode(REDLED, OUTPUT);
    pinMode(GREENLED, OUTPUT);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting...");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    
    server.on("/led_set", ledControl);
    server.on("/adcread", sensorData);

    server.begin();

    for(;;) {
        server.handleClient();
        //delay(1);
    }
}

void initer() {
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

void updater() {
    //Serial.println(count);
    count++;
}