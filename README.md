# Solar Tracker

This project uses an Arduino Uno to move a solar panel so it follows the sun. It uses four light sensors to find the brightest spot in the sky.

## Hardware

Board: Arduino Uno.

Shield: CNC Shield with A4988 drivers.

Motors: Two Nema17 stepper motors for side-to-side and up-and-down movement.

Sensors: Four photoresistors.

## Pin Settings

Left/Right Motor (Pan): Step pin 2, Direction pin 5.

Up/Down Motor (Tilt): Step pin 3, Direction pin 6.

Enable Pin: Pin 8 (must be LOW to run motors).

## Light Sensors:

Top Left: A0.

Top Right: A1.

Bottom Left: A2.

Bottom Right: A3.

## How it Works

The code has two main ways to work:

### 1. Auto Mode

In this mode, the Arduino reads the four light sensors. It compares the light on the top to the bottom, and the left to the right. If one side is brighter than the other by more than 50, the motors move the panel toward the light.

### 2. Manual Mode

In this mode, the Arduino waits for you to send commands through a USB cable. You can tell the motors exactly which way to turn and how many steps to take.
Commands

### Commands

You can send these text commands to the Arduino using a serial monitor at 115200 speed:

MODE:AUTO: Turns on the light-following mode.

MODE:MANUAL: Turns off the light-following mode so you can move it yourself.

STATUS: The Arduino sends back the current light sensor readings in a JSON format.

Movement: Use the format axis:direction:steps (for example, azimuth:clockwise:10).
