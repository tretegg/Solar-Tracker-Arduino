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

// true = uses LDRs to track automatically
// false = waits for manual commands over USB
bool isAutoMode = false;

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

void sendStatus() {
  int valTL = analogRead(PIN_TOP_LEFT);
  int valTR = analogRead(PIN_TOP_RIGHT);
  int valBL = analogRead(PIN_BOTTOM_LEFT);
  int valBR = analogRead(PIN_BOTTOM_RIGHT);

  // Manually construct the JSON response
  Serial.print("{\"ldr_top_left\": "); Serial.print(valTL);
  Serial.print(", \"ldr_bottom_right\": "); Serial.print(valBR);
  Serial.print(", \"ldr_bottom_left\": "); Serial.print(valBL);
  Serial.print(", \"ldr_top_right\": "); Serial.print(valTR);
  Serial.println("}"); // println adds the '\n' so Python knows it's finished
} 

void loop() {
  // Checks if there is a command over USB
  if (Serial.available() > 0) {
    // Read the incoming command until the newline character
    // Also converts the incoming bytes into a String
    String command = Serial.readStringUntil('\n');

    command.trim(); // Remove any leading/trailing whitespace

    // Mode switching commands
    if (command.equalsIgnoreCase("MODE:AUTO")) {
      isAutoMode = true;
      Serial.println("Switched to auto mode");
    } else if (command.equalsIgnoreCase("MODE:MANUAL")) {
      isAutoMode = false;
      Serial.println("Switched to manual mode");
    } 

    // Status command to send current sensor readings back to API
    else if (command.equalsIgnoreCase("STATUS")) {
      sendStatus();
    }
    
    // If we are in manual mode, parse the movement command
    else if (!isAutoMode) {
      // Look for the colons to split the string (e.g., "azimuth:clockwise:10")
      int firstColon = command.indexOf(':');
      int secondColon = command.lastIndexOf(':');

      // Extract the direction and distance
      String axis = command.substring(0, firstColon);
      String directionStr = command.substring(firstColon + 1, secondColon);
      int numSteps = command.substring(secondColon + 1).toInt();

      // Convert the direction to a boolean (clockwise = true, counterclockwise = false)
      bool direction = (directionStr == "clockwise");

      // Move the appropriate motor based on the axis
      if (axis == "azimuth") {
        for (int i = 0; i < numSteps; i++) {
          moveStep(stepX, dirX, direction);
        }
      } else if (axis == "elevation") {
        for (int i = 0; i < numSteps; i++) {
          moveStep(stepY, dirY, direction);
        }
      } else {
        Serial.println("Invalid command format. Use: azimuth:forward:10 or elevation:backward:5");
      }
    }
  }



  if (isAutoMode) {
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
}