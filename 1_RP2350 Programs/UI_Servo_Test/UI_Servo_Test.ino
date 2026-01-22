//Project Name: Processor Test 4
//Author: Ethan Conrad
//Reason: Testing RP2350 Processor
//Class: EE/CPE 490/491
//History: 11/17/2025 - Start Date
//
//Purpose: To create a program that can be used to perform the PWM output test on the selected processor.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

// === Pin Assignments ===
const int washoutIn = 26;
const int washoutSonarTrig = 8;
const int washoutSonarEcho = 9;
const int larvLevelSonarTrig = 16;
const int larvLevelSonarEcho = 18;
const int larvDumpMotor = 27;
const int uiServoPin = 19;   // <-- RENAMED PIN VARIABLE
const int batteryLevTrigPin = 4;


// Servo object
Servo uiServo;   // <-- RENAMED SERVO OBJECT

// === Setup ===
void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("=== RP2350 Beetle LED + Motor + Servo Test ===");
  // Servo init
  uiServo.attach(uiServoPin);
  Serial.println(uiServo.attached());
  uiServo.write(0);
  Serial.println("Servo starting at 0 degrees");
}

// === Main Loop ===
void loop() {
  Serial.println("Servo starting at 60 degrees");
  uiServo.write(60);
  delay(8000);
  Serial.println("Servo starting at 120 degrees");
  uiServo.write(120);
  delay(8000);
Serial.println("Servo starting at 180 degrees");
  uiServo.write(180);
  delay(8000);
Serial.println("Servo starting at 0 degrees");
  uiServo.write(0);
  delay(8000);
  
}
