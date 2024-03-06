#include <Arduino.h>
#include "sensor_def.h"


void setup() {
  // put your setup code here, to run once:
  // pin numbers for devices?
  // addresses?
  initializeSensors();
  initializeButtons();
}\

void loop() {
  // put your main code here, to run repeatedly:
  printLcd("Starting Test");
  delay(5000);
//  float humd = readHumidity();
//  String message = "Humidity: " + String(humd);
//  printLcd(message);
//  delay(5000);
//
//  float tmp = readTemp();
//  message = "Temp: " + String(tmp);
//  printLcd(message);
//  delay(5000);
//
//  float air = readAir();
//  message = "TVOC: " + String(air) + " ppm";
//  printLcd(message);
//  delay(5000);

    printHumidity(readHumidity());
    delay(5000);

//    printTemp(readTemp());
//    delay(5000);

    printAir(readAir());
    delay(5000);

   while(!buttonPressed(BUTTON_START)){
   }
   digitalWrite(LED_START, 1);
   digitalWrite(LED_IDLE, 0);
   printLcd("start button!");
   delay(5000);

   while(!buttonPressed(BUTTON_STOP)){
   }
   digitalWrite(LED_STOP, 1);
   digitalWrite(LED_START, 0);
   printLcd("stop button!");
   delay(5000);
}
