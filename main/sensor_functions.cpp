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
const int HEAT_PIN = 11;
const int SERVO_PIN = 6;
const int LED_IDLE = 8;
const int LED_STOP = 9;
const int LED_START = 10;
const int BUTTON_START = 3;
const int BUTTON_STOP = 2;
const int BLEND_PIN = 12;
volatile bool INTERRUPT = 0;
// bool IDLE = 1;
// bool AT_TEMP = 0;

Servo servo_lock;
Servo servo_blend;
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
    //while (1);
  } 

  if (!sgp.begin()){
    Serial.println("Air quality not found");
    //while (1);
  } 

 if (!tempsensor.begin(0x18)) {
   Serial.println("Temp not found");
   //while (1);
 } 


 tempsensor.setResolution(3); // set resolution of temp sensor
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  Serial.println("Trying to initialize LCD");
  lcd.begin(16,2);
  Serial.println("LCD begin called");
  //lcd.clear();
  Serial.println("All sensors initialized..");

}

void initializeButtons(){
  //Serial.begin(9600);
  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_STOP, INPUT);
  pinMode(LED_START, OUTPUT);
  pinMode(LED_STOP, OUTPUT);
  pinMode(LED_IDLE, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), estop, CHANGE);

  digitalWrite(LED_IDLE, 1); // idle led indicates device is on and not running
  digitalWrite(LED_START, 0);
  digitalWrite(LED_STOP, 0);
  // Serial.println("LEDs connected");

  // heating pin for relay
  pinMode(HEAT_PIN, OUTPUT);

  // Serial.println("Heating connected");

}

/**  READING FUNCTIONS  **/
float readHumidity(){
  return htu.readHumidity();
}

float readAir(){
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return -1;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reading air");
  lcd.setCursor(0, 1); // bottom left
  lcd.print("quality value...");
  
  // first 10-20 reading will be TVOC: 0 since the sensor is warming up
  int counter = 0;
  float non_zero_air = 0;
  float air = 0;
  float tolerance = 0.1;
  while (counter < 25 && INTERRUPT == 0){
    if (! sgp.IAQmeasure()) {
      Serial.println("Measurement failed");
      continue;
    }
    air = sgp.TVOC;
    // Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
    Serial.print("TVOC "); Serial.print(air); Serial.println(" ppb\t");
    // 0 to 400 ppb is acceptabl
    // Serial.print("Counter: "); Serial.println(counter);
    delay(500);
    counter++;
    if (air != 0) {
      non_zero_air = air;
    }
  }
  return max(non_zero_air, air);
}

float readAir2(){
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return -1;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reading air");
  lcd.setCursor(0, 1); // bottom left
  lcd.print("quality value...");
  delay(2000);

  float air = sgp.TVOC;
  Serial.print("TVOC "); Serial.print(air); Serial.println(" ppb\t");

  return air;
}

float readTemp(){
  tempsensor.wake();          // wake up mcp9808 for reading
  float c = tempsensor.readTempC();
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  return c;
}

/**  PRINT MSG FUNCTIONS  **/
void printAir(float air){
  String message = "TVOC: " + String(air) + " ppb";
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
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void printTempTime(float temp, float humd, double elapsed){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T(C):" + String(temp) + " H:" + String(humd));
  lcd.setCursor(0, 1);

  long totalSeconds = long(elapsed) / 1000;

  // Calculate hours, minutes, and remaining seconds
  int hours = totalSeconds / 3600;
  int remainingSeconds = totalSeconds % 3600;
  int minutes = remainingSeconds / 60;
  int seconds = remainingSeconds % 60;

  lcd.print("Time: " + String(hours) + ":" + String(minutes) + ":" + String(seconds));
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

/**  SERVO FUNCTIONS  **/
void setupServos(){
  Serial.begin(9600);
  servo_lock.attach(SERVO_PIN);
  servo_lock.write(180);
  Serial.println("Initialize Servo in Unlocked Position");
  // delay(15);
  servo_blend.attach(BLEND_PIN);
  servo_blend.write(90);    // blending servo is stationary
}

void Lock(){
  Serial.println("Locking..");
  delay(15);
  servo_lock.write(180-75);
}

void Unlock(){
  // Serial.println("Unlocking... return to home position");
  servo_lock.write(180);
}

void Blend(int seconds){
  servo_blend.write(135);
  delay(seconds*1000);
  servo_blend.write(90);
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

void estop(){
   digitalWrite(LED_STOP, 1);
   digitalWrite(LED_START, 0);
   digitalWrite(LED_IDLE, 0);
   INTERRUPT = 1;
//   lcd.clear();
//   lcd.print("estop triggered");
   servo_lock.write(180);
   servo_blend.write(90);
   heatOff();
}
