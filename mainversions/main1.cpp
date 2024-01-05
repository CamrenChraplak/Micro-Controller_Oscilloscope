#include <Arduino.h>
#include "test.h"
#include "esp32Wifi.h"


#ifdef ESP32
  #include "esp32Wifi.h"
  #include "test.h"
#endif
#ifdef UNO
  #include "test.h"
#endif

/*
#ifdef ETHERNET
  #include "esp32Wifi.h"
#endif
*/

void setup(void) {
  Serial.begin(115200);
  initer();
}

void loop(void) {
  updater();
  //Serial.println(getValue());
  //Serial.println("a");
}