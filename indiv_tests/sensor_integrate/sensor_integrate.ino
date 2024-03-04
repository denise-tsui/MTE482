#include <Arduino.h>
#include "sensor_def.h"

void setup() {
  // put your setup code here, to run once:
  // pin numbers for devices?
  // addresses?
  initializeSensors();
}

void loop() {
  // put your main code here, to run repeatedly:
  printLcd("Starting Test");
  delay(5000);
  float humd = readHumidity();
  String message = "Humidity: " + String(humd);
  printLcd(message);
  delay(5000);

  float tmp = readTemp();
  message = "Temp: " + String(tmp);
  printLcd(message);
  delay(50000);
  
  
}
