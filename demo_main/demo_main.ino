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
  if (INTERRUPT == 1){
    printLcd("estop triggered");
    Unlock();
    delay(10000);
  }
  
  printLcd("In IDLE state...");
  delay(2000);

  if (IDLE == 1 && INTERRUPT == 0) {
    while (!buttonPressed(BUTTON_START) && INTERRUPT == 0){}

    digitalWrite(LED_START, 1);
    digitalWrite(LED_IDLE, 0);
    printLcd("Cycle starting...");
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
    printTemp(readTemp());
  }
  
  if (PREHEAT == 1 && INTERRUPT == 0) {
    delay(5000);  // delay 5s before start
    printLcd("Begin preheating");
    heatOn();

    while (AT_TEMP != 1 && INTERRUPT == 0) {
//       double temp = readTemp();
//       printHumidity(readHumidity());
//      delay(3000);
//      printAir(readAir());
      delay(10000);    // check temp frequency (10s)
      printLcd("Reached temp");
      delay(4000);

      printTemp(readTemp());
      PREHEAT = 0;
      AT_TEMP = 1;
    }
  }

  if (AT_TEMP == 1 && INTERRUPT == 0) {
    // 1. hold at temp for set timed duration
    // 2. AT_TEMP = 0; COOLING = 1;
    unsigned long StartTime = millis();
    unsigned long elapsed = 0;
    unsigned long CurrentTime = 0;

    double minute = 2;
    double ms_in_min = 60000;
    double delay_duration = minute * ms_in_min;
    // delay(delay_duration);

    printLcd("Drying...");

    while (elapsed < delay_duration && INTERRUPT == 0) {
      Blend(10); // blend for 5 seconds
      printTempTime(readTemp(), readHumidity(), elapsed);
      delay(10000);
      printLcd("Drying...");
      delay(10000);
      CurrentTime = millis();
      elapsed = CurrentTime - StartTime;
      
    }

    printLcd("Heating done!");
    heatOff();
    delay(3000);
    AT_TEMP = 0;
    COOLING = 1;
  }

  if (COOLING == 1 && INTERRUPT == 0) {
    printLcd("Begin cooling..");
    // 1. temp is at safe temp < 35
    // 2. COOLING = 0; CYCLE_FINISHED = 1;
    delay(6000);  // wait n minutes until beginning to check temp sensor read
    while (CYCLE_FINISHED != 1 && INTERRUPT == 0) {
      //  first reading of cooling
      float temp = readTemp();
      printTemp(temp);
      if (temp < 40.0) {
        delay(5000);  
        printLcd("cooling... B)");
        delay(5000);   
        //  second reading of cooling
        temp = readTemp();
        printTemp(temp);
        delay(5000);  
        printLcd("cooling... B)");
        delay(5000);  
        //  third reading of cooling
        temp = readTemp();
        printTemp(temp);
        delay(5000);  // wait 5s until cycle finished
        COOLING = 0;
        CYCLE_FINISHED = 1;
      }
    }
  }

  if (CYCLE_FINISHED == 1 && INTERRUPT == 0) {
    printLcd("cycle finished");
    delay(5000);
    printLcd("quality summary:");
    printAir(readAir());
    
    delay(3000);
    printHumidity(readHumidity());
    delay(3000);

    
    // 1. print lcd about cycle finished etc.
    // 2. turn off after 2 minutes
    // 3. locking servo unlocks lid
    Unlock();
    delay(5000);
    printLcd("please open lid");
    delay(10000);
    CYCLE_FINISHED = 0;
    IDLE = 1;
  }

//  if (INTERRUPT == 1){
//    printLcd("estop triggered");
//  }
}
