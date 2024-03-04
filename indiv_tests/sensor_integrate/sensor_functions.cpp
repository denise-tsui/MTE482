//for LCD, air quality, humidity, and tempurature

#include <Arduino.h>
#include <Wire.h>
#include "sensor_def.h"
#include "Adafruit_HTU21DF.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>

Adafruit_SGP30 sgp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

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

  lcd.begin(16,2);
  lcd.clear();
}

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

void printLcd(String message){
  lcd.clear();
  lcd.print(message);
}
