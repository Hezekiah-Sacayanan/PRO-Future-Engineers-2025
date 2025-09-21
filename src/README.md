# Source Code

This folder contains all Arduino source code for the robot’s behavior and control logic.

## Structure

- `main.ino` – Main program for robot navigation, obstacle avoidance, and parking logic.
- `button.py` – RaspberryPi program for activating the color sensing program.
- `color.py` – Main RaspBerryPi program for the color detection of traffic blocks.

## Hardware Interfaces

- **color sensors** – Lap detection and line detection
- **Proximity sensors** – Traffic block and Wall detection
- **Servo motor** – Steering control
- **DC motor (via Gizduino motor shield)** – Forward/reverse motion
- **RPi camera** – Color detection for traffic blocks and parking zones
