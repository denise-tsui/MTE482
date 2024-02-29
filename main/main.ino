#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "sensor_functions.h"

int heat_pinOut = 10;
bool IDLE = 0;      // waiting for input
bool PREHEAT = 0;   // lid closed, preheating has begun
bool AT_TEMP = 0;   // temperature sensor shows @ max temp, start timer
bool COOLING = 0;   // when timer finished, allow x minutes to cool
bool CYCLE_FINISHED = 0;
bool START = 0;

void setup() {
  setup_serial_i2c();
  
  // initialize PTC heating & environment sensors
  setup_PTC();
  setup_Temp();
  setup_Lock();
}

void loop() {             
  if (IDLE == 1) {
    // while button not pressed, LCD display waiting for input
    // wait for start button pressed
    // 1. servo locks lid
    // 2. IDLE = 0; PREHEAT = 1;

    while (!START) {}
    Serial.println("Start button pushed. Beginning heating cycle...");
    delay(1000);    
    // assume start button pressed only after lid is closed
    Lock();
    IDLE = 0; PREHEAT = 1;

  }

  if (PREHEAT == 1) {
    delay(15000);  // delay 15s before start
    heatOn();

    while (AT_TEMP != 1) {
      double temp = read_Temp();
      if (temp > 140) {
        PREHEAT = 0;
        AT_TEMP = 1;
      }
      delay(30000);    // check temp frequency
    }
  }

  if (AT_TEMP == 1) {
    // 1. hold at temp for set timed duration
    // 2. AT_TEMP = 0; COOLING = 1;
    double minute = 15;
    double ms_in_min = 60000;
    double delay_duration = minute * ms_in_min;
    delay(delay_duration);
    AT_TEMP = 0;
    COOLING = 1;
  }

  if (COOLING == 1) {
    // 1. temp is at safe temp < 35
    // 2. COOLING = 0; CYCLE_FINISHED = 1;
    delay(150000);  // wait n minutes until beginning to check temp sensor read
    while (CYCLE_FINISHED != 1) {
      double temp = read_Temp();
      if (temp > 60) {
        delay(30000);   // wait 30s until next check
        // (reduce check time from every 10 minutes -> 5 minutes -> 30 seconds -> 5 seconds)
      }
      if (temp < 35) { 
        COOLING = 0; 
        CYCLE_FINISHED = 1; 
      }
    }
  }

  if (CYCLE_FINISHED == 1) {
    // 1. print lcd about cycle finished etc.
    // 2. turn off after 2 minutes
    // 3. locking servo unlocks lid

    Unlock();

  }
}