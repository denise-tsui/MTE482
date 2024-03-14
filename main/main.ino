#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "sensor_definitions.h"

bool IDLE = 1;      // waiting for input
bool PREHEAT = 0;   // lid closed, preheating has begun
bool AT_TEMP = 0;   // temperature sensor shows @ max temp, start timer
bool COOLING = 0;   // when timer finished, allow x minutes to cool
bool CYCLE_FINISHED = 0;
bool START = 0;

void setup() {
  initializeSensors();
  initializeButtons();
  setupServos();
  // attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), estop, CHANGE);
}

void loop() {  
  printLcd("Starting Test");
  delay(2000);

  if (IDLE == 1) {
    while (!buttonPressed(BUTTON_START)){}

    digitalWrite(LED_START, 1);
    digitalWrite(LED_IDLE, 0);
    printLcd("start button!");
    // while button not pressed, LCD display waiting for input
    // wait for start button pressed
    // 1. servo locks lid
    // 2. IDLE = 0; PREHEAT = 1;

    // Serial.println("Start button pushed. Beginning heating cycle...");
    delay(1000);    
    // assume start button pressed only after lid is closed
    Lock();
    IDLE = 0; 
    PREHEAT = 1;
  }

  if (PREHEAT == 1) {
    delay(8000);  // delay 8s before start
    heatOn();
    printLcd("begin heating");

    while (AT_TEMP != 1) {
      // double temp = readTemp();
      // if (temp > 140) {
      //   PREHEAT = 0;
      //   AT_TEMP = 1;
      // }
      printHumidity(readHumidity());
      delay(10000);    // check temp frequency (10s)
      AT_TEMP = 1;
      printLcd("reached temp");
    }
  }

  if (AT_TEMP == 1) {
    // 1. hold at temp for set timed duration
    // 2. AT_TEMP = 0; COOLING = 1;
    unsigned long StartTime = millis();
    unsigned long elapsed = 0;
    unsigned long CurrentTime = 0;

    double minute = 1;
    double ms_in_min = 60000;
    double delay_duration = minute * ms_in_min;
    // delay(delay_duration);
    
    // while (elapsed < delay_duration) {
      Blend(5); // blend for 5 seconds
      delay(5000);
      Blend(5); // blend for 5 seconds
      delay(5000);
      Blend(5); // blend for 5 seconds
      delay(5000);
      Blend(5); // blend for 5 seconds
      delay(5000);
      // CurrentTime = millis();
      // elapsed = CurrentTime - StartTime;
    // }

    printLcd("done heat");
    heatOff();
    delay(500);
    AT_TEMP = 0;
    COOLING = 1;
  }

  if (COOLING == 1) {
    printLcd("begin cool");
    // 1. temp is at safe temp < 35
    // 2. COOLING = 0; CYCLE_FINISHED = 1;
    delay(30000);  // wait n minutes until beginning to check temp sensor read
    while (CYCLE_FINISHED != 1) {
      double temp = readTemp();
      // if (temp > 60) {
        delay(30000);   // wait 30s until next check
        // (reduce check time from every 10 minutes -> 5 minutes -> 30 seconds -> 5 seconds)
      // }
      if (temp < 35) { 
        COOLING = 0; 
        CYCLE_FINISHED = 1; 
      }
    }
  }

  if (CYCLE_FINISHED == 1) {
    while(!buttonPressed(BUTTON_STOP)) {
      printLcd("cycle finished");
      delay(10000);
      printLcd("quality summary:");
      printAir(readAir());
      delay(5000);
      printHumidity(readHumidity());
      delay(5000);

      
      // 1. print lcd about cycle finished etc.
      // 2. turn off after 2 minutes
      // 3. locking servo unlocks lid
      Unlock();
      delay(5000);
      printLcd("please open lid");
      IDLE = 1;
    } 
  }
}
