/*!
 * @file sensor_functions.cpp
 * @brief Compilation of sensor function definitions
 *
 * @author 
 * @version  V1.0
 * @date  2023-03-17
 */

#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_MCP9808.h"

// define connections and objects
const int HEAT_PIN = 10;
Adafruit_MCP9808 temp = Adafruit_MCP9808();

/**************************************/
/*            functions               */
/**************************************/

// setup serial communication and join i2c bus
void setup_serial_i2c(){
  // common for wire and serial
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
}

void setup_PTC(){
  // The heat pin will tell the relay to initialize
  pinMode(HEAT_PIN, OUTPUT);
}

void setup_Temp(){
  // check for sensor found
  if (!temp.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check connections and verify address is correct.");
    while (1);
  }
  
  Serial.println("Found MCP9808!");
  temp.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
}

float read_Temp(){
  temp.wake();          // wake up mcp9808 for reading
  float c = temp.readTempC();
  Serial.print("Temp: ");
  Serial.print(c, 4); Serial.print("*C\t.");
  delay(500);
  temp.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
}

void heatOn(){
  // The heat pin will tell the relay to initialize
  digitalWrite(HEAT_PIN, HIGH);
}

void heatOff(){
  // The heat
  pinMode(HEAT_PIN, LOW);
}



