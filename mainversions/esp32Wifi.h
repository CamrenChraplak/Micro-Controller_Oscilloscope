#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ESPAsyncWebServer.h"
#include "index.h"
#include "SPIFFS.h"
#include "wifi_credentials.h"

void initer();
void updater();