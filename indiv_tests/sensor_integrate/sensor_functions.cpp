//for LCD, air quality, humidity, and tempurature

#include <Arduino.h>
#include <Wire.h>
#include "sensor_def.h"
#include "Adafruit_HTU21DF.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>

const int LED_START = 8;
const int LED_IDLE = 9;
const int LED_STOP = 10;
const int BUTTON_START = 7;
const int BUTTON_STOP = 11;

unsigned long debounceDelay = 50;
int buttonState;
int lastButtonState = 0;

Adafruit_SGP30 sgp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// initialization
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

//  if (!tempsensor.begin(0x18)) {
//    Serial.println("Temp not found");
//    while (1);
//  }
//  tempsensor.setResolution(3); // set resolution of temp sensor

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
}

// reading
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
  tempsensor.wake(); 
  float c = tempsensor.readTempC();
  tempsensor.shutdown_wake(1);
  return c;
}

// printing
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
