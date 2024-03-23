#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "sensor_definitions.h"

bool IDLE = 1;      // waiting for input
bool IN_CYCLE = 0;   // lid closed, preheating has begun
bool CYCLE_FINISHED = 0;
bool START = 0;

void setup() {
  initializeSensors();
  initializeButtons();
  // setupServos();
  // attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), estop, CHANGE);
}

void loop() {  
  if (INTERRUPT == 1){
    printLcd("estop triggered");
    Unlock();
    delay(10000);
  }
  
  printLcd("Ready?");
  delay(2000);

  if (IDLE == 1 && INTERRUPT == 0) {
    while (!buttonPressed(BUTTON_START) && INTERRUPT == 0){}

    digitalWrite(LED_START, 1);
    digitalWrite(LED_IDLE, 0);
    printLcd("Starting cycle...");
    delay(3000);
    // while button not pressed, LCD display waiting for input
    IDLE = 0;
    IN_CYCLE = 1;
  }
  
  if (IN_CYCLE == 1 && INTERRUPT == 0) {
    // delay(5000);  // delay 5s before start
    // printLcd("Preheating..");
    // delay(10000000);    // check temp frequency (10s)
    // int n_hours = 0;
    String message = "8 hours left...";
    printLcd(message); 
    delay(12000);

    for (int i = 7; i > 1; i--) {
      String message = String(i) + " hours left...";
      printLcd(message); 
      delay(3000);
    }
    `message = "1 hour left...";
    printLcd(message); 
    delay(4000);
    IN_CYCLE = 0;
    CYCLE_FINISHED = 1;
}

  if (CYCLE_FINISHED == 1 && INTERRUPT == 0) {
    printLcd("Cycle finished!");
    delay(5000);
    printLcd("Please open lid");
    delay(10000);
    digitalWrite(LED_START, 0);
    digitalWrite(LED_IDLE, 1);
    CYCLE_FINISHED = 0;
  }
}
//
