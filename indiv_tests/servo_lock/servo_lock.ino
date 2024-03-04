#include <Servo.h>

Servo servo;
const int SERVO_PIN = 8;
bool READY = 0;
bool LOCKED = 0;

void setup() {
  // put your setup code here, to run once:
  servo.attach(8);
  servo.write(45);
  Serial.begin(9600);          // start serial communication at 9600bps

  Serial.println("Reset Servo to Zero");

  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (LOCKED == 1) {
    Serial.println("Unlocking..");
    servo.write(45);
    LOCKED = 0;
    READY = 0;
  }

  if (READY == 1) {
    Serial.println("Locking..");
    delay(500);
    servo.write(135);
    delay(5000);
    LOCKED = 1;
  } else {
    Serial.println("Wait until ready..(7s)");
    delay(7000);
    READY = 1;
    Serial.println("Ready to lock..");
  }

  delay(100);

}
