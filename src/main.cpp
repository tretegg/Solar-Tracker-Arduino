#include <Arduino.h>

// Change this to the number of steps on your motor 
#define STEPS 200

// CNC Shield pin definitions for the A4988 stepper drivers
// X header
const int stepX = 2; // Step pin for Pan
const int dirX = 5;  // Direction pin for Pan
// Y header
const int stepY = 3; // Step pin for Tilt
const int dirY = 6;  // Direction pin for Tilt

const int enablePin = 8; // Enabled pin (shared)

// Define the analog pins for the 4 photoresistors
const int PIN_TOP_LEFT = A0;
const int PIN_TOP_RIGHT = A1;
const int PIN_BOTTOM_LEFT = A2;
const int PIN_BOTTOM_RIGHT = A3;

// Variables to store sensor readings
int valTL, valTR, valBL, valBR;

// Tolerance prevents the motors from jittering if the light difference is tiny
const int TOLERANCE = 50; 

void setup() {
  Serial.begin(115200); // Start serial communication for debugging

  // Set all motor pins as outputs
  // Outputs power to the motors instead of expecting input from them
  pinMode(stepX, OUTPUT);
  pinMode(dirX, OUTPUT);
  pinMode(stepY, OUTPUT);
  pinMode(dirY, OUTPUT);
  
  // The Enable pin MUST be LOW to power the motors
  // LOW = outputs enabled, motor coils driven
  // HIGH = outputs disabled, motor coils off
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

// Function to move a motor exactly one step
void moveStep(int stepPin, int dirPin, bool direction) {
  // HIGH (true) = clockwise, LOW (false) = counterclockwise
  
  // Set direction
  digitalWrite(dirPin, direction);

  // Step the motor
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(1000);
}

void loop() {
  int valTL = analogRead(PIN_TOP_LEFT);
  int valTR = analogRead(PIN_TOP_RIGHT);
  int valBL = analogRead(PIN_BOTTOM_LEFT);
  int valBR = analogRead(PIN_BOTTOM_RIGHT);

  int avgTop = (valTL + valTR) / 2;
  int avgBottom = (valBL + valBR) / 2;
  int avgLeft = (valTL + valBL) / 2;
  int avgRight = (valTR + valBR) / 2;

  // Vertical Movement (Tilt)
  if (abs(avgTop - avgBottom) > TOLERANCE) {
    moveStep(stepY, dirY, (avgTop > avgBottom)); 
  }

  // Horizontal Movement (Pan)
  if (abs(avgLeft - avgRight) > TOLERANCE) {
    moveStep(stepX, dirX, (avgLeft > avgRight));
  }

  delay(50); // Small delay for stability
}