//Project Name: Drain Defenders Processor Code
//Author: Ethan Conrad
//Reason: Control Drain Defenders Device
//Class: EE/CPE 490/491
//History: 10/30/2025 - Start Date
//         11/03/2025 - Adjusted pin definitions to match RP2350 actual pin layout, changed washoutIn to be a
//                      a digital input instead of analog to allow for interrupt usage, and added UI servo motor
//                      logic to work with one servo instead of two, and added servoCheck function to check
//                      servo states for the "both" state
//         11/04/2025 - Asjusted pin definitions to allow battery interrrupt sense to have I2C compatibility
//         11/10/2025 - Changed larvDumpMotor to larvDumpMotor due to realzing the RP2350 pins cannot supply in
//                      rush current needed to turn motor sufficiently. Then updated all code relevant to motor
//                      to digital commands to support MOSFET circuit logic
//         11/11/2025 - Adjusted duration of the low power sleep to be 5 minutes or 300000 ms
//         11/24/2025 - Changed Washout Sensor Interupt to falling edge based on results from testing script
//         12/02/2025 - Included Adafuit library and associated code/logic to enable I2C communication with Power
//                      Distribution subsystem
//         12/03/2025 - B.H.: Added I2C initialization and LC709203F/interrupt communication
//         12/08/2025 - added in set clock and data initialization code to fix I2C errors
//
//Purpose: To create a program that can be used to control the device that the Drain Defenders design for their
//             project sponsor Todd Montgomery. This device should be able to detect water flowing into a
//             water basin and dispense larvicide as needed. This code shall be able to receive and interface
//             with all necessary IO's from the different subsystems and process them correctly.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Servo.h>
#include <Wire.h>
#include "Adafruit_LC709203F.h"

// === Pin Definitions ===
const int washoutIn = 26;       // Digital pin to detect high signal (interrupt) from washout sensor
const int washoutSonarTrig = 8;              // Washout Sonar trigger pin
const int washoutSonarEcho = 9;              // Washout Sonoar echo pin
const int larvLevelSonarTrig = 16;              // Larvicide Level trigger pin (sonar)
const int larvLevelSonarEcho = 18;              // Larvicide Level echo pin (sonar)
const int larvDumpMotor = 27;               // Larvicide Level Dump Motor (PWM pin for N-Type MOSFET)
const int uiServo = 19;                // UI Battery Control Servo Motor
const int batteryLevTrigPin = 1;       // Battery Level Digital Input (interrupt)
const int batteryLevClkPin = 5;       // Battery Level Digital Input (interrupt)
const int batteryLevDataPin = 4;       // Battery Level Digital Input (interrupt)

// === Objects and Globals ===
Servo servoMotor; //Create Servo motor
Adafruit_LC709203F lc; //Create Fuel guage object
volatile bool washoutTrigger = false;   // boolean for washout interrupt trigger
volatile bool batteryTrigger = false; //boolean for battery interrupt trigger
float previousDistance = -1.0;         // Store last distance for washoutSonar, initialized to -1 for system boot
const float larvicideThreshold = 40.0;    // Preprogrammed value for larvicide level
volatile int larveCheck = 0; // Store checking variable for larvicide level
volatile int battCheck = 0; // Store checking variable for battery level
// === Utility: Measure distance from an HC-SR04-style sensor ===
float getSonarDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);// Writing Sequences for gaining
  long duration = pulseIn(echoPin, HIGH, 30000); // measure timeout in µs
  Serial.println(duration);
  float distance = (duration * 0.0343) / 2.0;      // ocnvert distance to cm
  return distance;//return distance
}//end getSonarDistance

// === Interrupt handlers ===
//Washout Interupt function
void washoutInterruptHandler() {//removed IRAM_ATTR
Serial.println("Washout Sensed");
  washoutTrigger = true;// flip boolean for execution in main loop
}//end washoutInterruptHandler

void batteryDigitalInterruptHandler() {//removed IRAM_ATTR
Serial.println("Battery Low Sensed");
    batteryTrigger = true;// flip boolean for execution in main loop
}//end batteryDigitialInterruptHandler

// === Utility: Move Servo to Final State if conditon given ===
void servoCheck() {
      if(larveCheck == 1 && battCheck == 1){
            servoMotor.write(100);
            larveCheck = 0;
            //battCheck = 0;
      }//end if
}//end servo check

// === Setup ===
void setup() {
  // Serial optional for debugging
  Serial.begin(9600);
  delay(2000);
  // Configure all pins
  pinMode(washoutIn, INPUT);
  pinMode(washoutSonarTrig, OUTPUT);
  pinMode(washoutSonarEcho, INPUT);
  pinMode(larvLevelSonarTrig, OUTPUT);
  pinMode(larvLevelSonarEcho, INPUT);
  pinMode(larvDumpMotor, OUTPUT);
  digitalWrite(larvDumpMotor, LOW);//Ensure MOSFET is closed at start
  pinMode(batteryLevTrigPin, INPUT_PULLUP);
  Serial.println("Begin Initialize");
 
  Wire.setSDA(batteryLevDataPin);
  Wire.setSCL(batteryLevClkPin);
  Wire.setClock(10000);
  Wire.begin();
  // Attach battery servo to project
  servoMotor.attach(uiServo);
  servoMotor.write(0);  // Start at 0%
  // Set interrupts
  attachInterrupt(digitalPinToInterrupt(batteryLevTrigPin), batteryDigitalInterruptHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(washoutIn), washoutInterruptHandler, FALLING);
  // Analog interrupt: use pin change or poll threshold
  washoutTrigger = false;
  batteryTrigger = false;
  //
  while (!lc.begin(&Wire)) {
    Serial.println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
    delay(10000);
  }
  Serial.println(F("Found LC709203F"));
  lc.setThermistorB(3950);
  lc.setPackSize(LC709203F_APA_3000MAH);
  lc.setAlarmRSOC(20);

  // Low power setup (optional: external libs like LowPower.h can deepen sleep)
  Serial.println("System initialized, entering low power wait...");
}//end steup function

// === Main loop ===
void loop() {
  // Check if battery is sensed to be low (interrupt trigger)
  if (batteryTrigger) {
    batteryTrigger = false;// reset boolean
    servoMotor.write(33);  // Move servo to 100%
    battCheck = 1;
    delay(1000);//give it time to move
    servoCheck();
  }
  // Check if washout event is sensed (interrupt trigger)
  if (washoutTrigger) {
    washoutTrigger = false; //reset boolean
    //Measure distance from device to water in water basin
    float devToWaterDist = getSonarDistance(washoutSonarTrig, washoutSonarEcho);//store distance in distance variable
    Serial.print("Sonar1 distance: "); Serial.println(devToWaterDist);//debug statement to print data
    if (previousDistance < 0 || abs(devToWaterDist - previousDistance) > 1.0) { // Change detected
      digitalWrite(larvDumpMotor, HIGH); // Run motor full power
      Serial.print("Dispensing Larvicide");
      delay(4000);               // 2 seconds, need to edit for how long a scoop is
      digitalWrite(larvDumpMotor, LOW);   // Stop motor
      // Measure larivicde level
      float sensToLarvDistance = getSonarDistance(larvLevelSonarTrig, larvLevelSonarEcho);//find distance from sonar to larvicide level
      Serial.print("Sonar2 distance: "); Serial.println(sensToLarvDistance);//debug statement to print data
      if (sensToLarvDistance < larvicideThreshold) {//If larvicide is less than threshold indicate it on the UI
         servoMotor.write(66);
         delay(1000);
         larveCheck = 1;
         servoCheck();
      }//end if ()
    }//end if (dispense)
    previousDistance = devToWaterDist;//update and store new distance
  }//end if
  
  // === Low Power Wait ===
  //delay(300000); // short delay; replace with LowPower.sleep() for deep sleep, in ms,
  Serial.println("Sleeping");// This and below delay statement for debugging
  delay(1000);
}