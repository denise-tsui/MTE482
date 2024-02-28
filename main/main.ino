#include <math.h>
#include <Arduino.h>
#include <Wire.h>
#include "sensor_functions.h"

int heat_pinOut = 10;
bool IDLE = 0;    // waiting for input
bool PREHEAT = 0;
bool AT_TEMP = 0;
bool COOLING = 0;
bool CYCLE_FINISHED = 0;

void setup() {
  setup_serial_i2c();
  
  // initialize PTC heating & environment sensors
  setup_PTC();
  setup_Temp();
}

void loop() {             
  if (IDLE == 1) {
    // while button not pressed, LCD display waiting for input
    // wait for start button pressed
    // 1. servo locks lid
    // 2. IDLE = 0; PREHEAT = 1;

  }

  if (PREHEAT == 1) {
    double minute = 15;
    double ms_in_min = 60000;
    double delay_duration = minute * ms_in_min;
    delay(15000);  // delay 15s before start
    heatOn();

    double temp = read_Temp();
    delay(delay_duration);

    // finish 3 minutes of heating & turn off
    // cool down period
    heatOff();
    delay(delay_duration);
  }

  if (AT_TEMP == 1) {
    // 1. hold at temp for set timed duration
    // 2. AT_TEMP = 0; COOLING = 1;
  }

  if (COOLING == 1) {
    // 1. temp is at safe temp < 35
    // 2. COOLING = 0; CYCLE_FINISHED = 1;
  }

  if (CYCLE_FINISHED == 1) {
    // 1. print lcd about cycle finished etc.
    // 2. turn off after 2 minutes
    // 3. locking servo unlocks lid
  }
}