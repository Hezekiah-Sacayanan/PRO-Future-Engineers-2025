# Source Code

This folder contains all Arduino source code for the robot’s behavior and control logic.

## Structure

- `main.ino` – Main program for robot navigation, obstacle avoidance, and parking logic.
- `button.py` – RaspberryPi program for activating the colorsensing program.
- `color.py` – Main RaspBerryPi program for the color detection of traffic blocks.

## Hardware Interfaces

- **IR sensors** – Line following and directional turns
- **Proximity sensor** – Traffic block detection and parking triggers
- **Servo motor** – Steering control
- **DC motor (via Gizduino motor shield)** – Forward/reverse motion
- **RPi camera** – Color detection for traffic blocks and parking zones
- **IMU sensor** – Lap detection via yaw tracking
