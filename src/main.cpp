#include <Arduino.h>
#include <Stepper.h>

// Change this to the number of steps on your motor 
#define STEPS 100

// Create TWO stepper instances for Pan (Left/Right) and Tilt (Up/Down)
Stepper panMotor(STEPS, 8, 9, 10, 11);
Stepper tiltMotor(STEPS, 4, 5, 6, 7); 

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

  // Set the speed of the stepper motors (RPM)
  panMotor.setSpeed(30);
  tiltMotor.setSpeed(30);
}

void loop() {
  // Read the raw analog values
  valTL = analogRead(PIN_TOP_LEFT);
  valTR = analogRead(PIN_TOP_RIGHT);
  valBL = analogRead(PIN_BOTTOM_LEFT);
  valBR = analogRead(PIN_BOTTOM_RIGHT);

  // Calculate the averages for each half of the sensor array
  int avgTop = (valTL + valTR) / 2;
  int avgBottom = (valBL + valBR) / 2;
  int avgLeft = (valTL + valBL) / 2;
  int avgRight = (valTR + valBR) / 2;

  // Determine Tilt (Up/Down)
  int diffVertical = avgTop - avgBottom;
  
  if (abs(diffVertical) > TOLERANCE) {
    if (avgTop > avgBottom) {
      tiltMotor.step(1); // Move Up
    } else {
      tiltMotor.step(-1); // Move Down
    }
  }

  // Determine Pan (Left/Right)
  int diffHorizontal = avgLeft - avgRight;
  
  if (abs(diffHorizontal) > TOLERANCE) {
    if (avgLeft > avgRight) {
      panMotor.step(1); // Move Left
    } else {
      panMotor.step(-1); // Move Right
    }
  }

  // Wait 100ms before next reading
  delay(100);
}