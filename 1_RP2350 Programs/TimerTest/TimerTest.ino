//Project Name: Processor Test 3
//Author: Ethan Conrad
//Reason: Testing RP2350 Processor
//Class: EE/CPE 490/491
//History: 11/17/2025 - Start Date
//
//Purpose: To create a program that can be used to perform the timer usage test on the selected processor.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
///test 3

const int LED_PIN = 0;           // Any valid GPIO on Beetle RP2350
unsigned long lastTime = 0;      // timestamp
const unsigned long interval = 5000; // 5 seconds

void setup() {
    Serial.begin(9600);
    delay(500);                    // Give USB serial time to enumerate
    Serial.println("=== Beetle RP2350 Timer Test Started ===");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    unsigned long now = millis();

    // Check if 5 seconds passed
    if (now - lastTime >= interval) {
        lastTime = now;   // reset timer

        Serial.println("Timer fired → LED pulse");

        // Pulse LED
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
    }

    // Idle CPU lightly (compatible with RP2350)
    __wfi();     // Wait for interrupt (safe low-power idle)
}
