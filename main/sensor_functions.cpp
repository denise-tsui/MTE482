/*!
 * @file sensor_functions.cpp
 * @brief Compilation of sensor function definitions
 *
 * @author 
 * @version  V1.0
 * @date  2023-03-17
 */

#include <Arduino.h>
#include <Wire.h>
#include "sensor_definitions.h"
#include "Adafruit_HTU21DF.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


/** CONNECTION PINS AND OBJECT DEFINITION   **/
const int HEAT_PIN = 5;
const int SERVO_PIN = 6;
const int LED_START = 8;
const int LED_IDLE = 9;
const int LED_STOP = 10;
const int BUTTON_START = 7;
const int BUTTON_STOP = 11;

Servo servo;
Adafruit_SGP30 sgp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/** INITIALIZATION OF VARIABLES   **/
unsigned long debounceDelay = 50;
int buttonState;
int lastButtonState = 0;

/**  INITIALIZATION  **/
void initializeSensors() {
  // Wire.begin();
  Serial.begin(9600);
  Serial.println("Sensor Initialization");

  if (!htu.begin()) {
    Serial.println("Humidity not found");
    while (1);
  }
  if (!sgp.begin()){
    Serial.println("Air quality not found");
    while (1);
  }

 if (!tempsensor.begin(0x18)) {
   Serial.println("Temp not found");
   while (1);
 }
 tempsensor.setResolution(3); // set resolution of temp sensor
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  lcd.begin(16,2);
  lcd.clear();
}

void initializeButtons(){
  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_STOP, INPUT);
  pinMode(LED_START, OUTPUT);
  pinMode(LED_STOP, OUTPUT);
  pinMode(LED_IDLE, OUTPUT);

  digitalWrite(LED_IDLE, 1); // idle led indicates device is on and not running
  digitalWrite(LED_START, 0);
  digitalWrite(LED_STOP, 0);

  // heating pin for relay
  pinMode(HEAT_PIN, OUTPUT);

}

/**  READING FUNCTIONS  **/
float readHumidity(){
  return htu.readHumidity();
}

float readAir(){
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return 0;
  }
  return sgp.TVOC;
}

float readTemp(){
  tempsensor.wake();          // wake up mcp9808 for reading
  float c = tempsensor.readTempC();
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  return c;
}

/**  PRINT MSG FUNCTIONS  **/
void printAir(float air){
  String message = "TVOC: " + String(air) + " ppm";
  printLcd(message);
}

void printTemp(float temp){
  String message = "Temp: " + String(temp);
  printLcd(message);
}

void printHumidity(float hmd){
  String message = "Humidity: " + String(hmd);
  printLcd(message);
}

void printLcd(String message){
  lcd.clear();
  lcd.print(message);
}

/**  PTC HEATING FUNCTIONS  **/
void heatOn(){
  // The heat pin will tell the relay to initialize
  digitalWrite(HEAT_PIN, HIGH);
}

void heatOff(){
  // The heat
  pinMode(HEAT_PIN, LOW);
}

/**  LOCKING SERVO FUNCTIONS  **/
void setupServo(){
  servo.attach(SERVO_PIN);
  servo.write(45);
  // Serial.println("Initialize Servo in Unlocked Position");
  // delay(15);
}

void Lock(){
  // Serial.println("Locking..");
  delay(15);
  servo.write(135);
}

void Unlock(){
  // Serial.println("Unlocking... return to home position");
  servo.write(45);
}

bool buttonPressed(int buttonPin){
  int reading = digitalRead(buttonPin);
  unsigned long lastDebounceTime = 0;

  // button is pressed or noise detected
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  // if the reading is longer than the debounce time, assume it is pressed
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        return 1;
      }
    }
  }
  lastButtonState = reading;
  return 0;
}


