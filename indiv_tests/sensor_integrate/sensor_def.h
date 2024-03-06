//for LCD, air quality, humidity, and tempurature

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
void printLcd();

bool buttonPressed(int buttonPin);
