//Project Name: Processor Test 2 and 5
//Author: Ethan Conrad
//Reason: Testing RP2350 Processor
//Class: EE/CPE 490/491
//History: 11/17/2025 - Start Date
//
//Purpose: To create a program that can be used to perform the interrupt usage and transistor utilization tests 
//               on the selected processor.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>

// === Pin Assignments ===
const int washoutIn = 26;      // Interrupt input pin (3.3V pulse)
const int led1Sec = 0;         // LED1 (interrupt indicator)
const int larvDumpMotor = 27;  // Motor digital output (pulse)

// === Interrupt Tracking ===
volatile bool interruptTriggered = false;
unsigned long interruptTime = 0;

// === Interrupt Handler ===
void onPulseDetected() {
  interruptTriggered = true;   // Notify main loop
  interruptTime = millis();    // Record time
}

// === Setup ===
void setup() {
  Serial.begin(9600);
  delay(200);
  Serial.println("=== RP2350 Beetle Interrupt Test ===");

  pinMode(led1Sec, OUTPUT);
  digitalWrite(led1Sec, LOW);

  pinMode(larvDumpMotor, OUTPUT);
  digitalWrite(larvDumpMotor, LOW);   // Start LOW

  pinMode(washoutIn, INPUT);
  attachInterrupt(digitalPinToInterrupt(washoutIn), onPulseDetected, RISING);

  Serial.println("Interrupt armed on pin 26 (RISING).");
}

// === Main Loop ===
void loop() {
  unsigned long now = millis();

  // ==========================
  // Test 2: Interrupt Handling
  // ==========================
  if (interruptTriggered) {
    Serial.println("Interrupt detected!");
    digitalWrite(led1Sec, HIGH);
    interruptTriggered = false;
  }

  // Auto-turn-off LED1 after 500 ms
  if (digitalRead(led1Sec) == HIGH && (now - interruptTime >= 500)) {
    digitalWrite(led1Sec, LOW);
    Serial.println("LED1 OFF (auto reset)");
  }

  // ==========================
  // Test 5: BJT Switching
  // ==========================
  const unsigned long HIGH_DURATION = 5000;  // 5 seconds HIGH
  const unsigned long LOW_DURATION  = 5000;  // 5 seconds LOW
  const unsigned long TOTAL_CYCLE   = HIGH_DURATION + LOW_DURATION;

  unsigned long cyclePos = now % TOTAL_CYCLE;

  if (cyclePos < HIGH_DURATION) {
    digitalWrite(larvDumpMotor, HIGH);    // HIGH for 5 seconds
  } else {
    digitalWrite(larvDumpMotor, LOW);     // LOW for 5 seconds
  }
}
