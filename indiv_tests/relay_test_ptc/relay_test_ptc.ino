#include <math.h>

int pinOut = 10;

double Thermistor(int RawADC) {
 double Temp;
 Temp = log(10000.0*((1024.0/RawADC-1))); 
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;          
 Temp = (Temp * 9.0)/ 5.0 + 32.0; 
 return Temp;
}

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
}

void loop() {             
  bool begin_heat = false;
  double minute = 15;
  double ms_in_min = 60000;
  double delay_duration = minute * ms_in_min;
  delay(15000);  // delay 15s before start
  begin_heat = true;
  digitalWrite(pinOut, HIGH);
  // }
  // else {
  //   digitalWrite(pinOut, HIGH);
  // }
  delay(delay_duration);

  // finish 3 minutes of heating & turn off
  digitalWrite(pinOut, LOW);
  delay(delay_duration);
}