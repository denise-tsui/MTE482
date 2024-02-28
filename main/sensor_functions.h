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

// setup PTC heating
void setup_PTC();

// setup MCP9808 sensor
void setup_Temp();

// read MCP9808 temp
float read_Temp();

// set PTC to high 
void heatOn();

// set PTC to low 
void heatOff();

#endif
