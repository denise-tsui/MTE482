/*!
 * @file sensor_functions.h
 * @brief Compilation of sensor function prototypes
 *
 * @author 
 * @version  V1.0
 * @date  2024-02-28
 */

#ifndef sensor_functions_h
#define sensor_functions_h

// setup serial communication and join i2c bus
void setup_serial_i2c();

// setup MCP9808 sensor
void setup_Temp();
float read_Temp();

// setup PTC heating
void setup_PTC();
void heatOn();  // set PTC to high 
void heatOff(); // set PTC to low

// setup locking servo
void setup_Lock();
void Lock();
void Unlock();

#endif
