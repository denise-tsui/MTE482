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

extern const int LED_START;
extern const int LED_IDLE;
extern const int LED_STOP;
extern const int BUTTON_START;
extern const int BUTTON_STOP;

// initialization
void initializeSensors();
void initializeButtons();

// reading
float readHumidity();
float readAir();
float readTemp();

// printing
void printLcd(String message);
void printAir(float air);
void printTemp(float temp);
void printHumidity(float hmd);

// PTC heating
void heatOn();  // set PTC to high 
void heatOff(); // set PTC to low

// servo
void setupServo();
void Lock();
void Unlock();

bool buttonPressed(int buttonPin);

#endif
