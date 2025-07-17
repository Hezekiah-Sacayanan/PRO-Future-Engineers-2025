# Source Code

This folder contains all Arduino source code for the robot’s behavior and control logic.

## Structure

- `main.ino` – Entry point of the program, contains the main control loop
- `functions.ino` – Support functions such as movement, turning, sensor handling
- `setup.ino` – Initialization of pins, motors, sensors, and camera communication
- `constants.h` – Pin assignments, motor speeds, and threshold values

## Hardware Interfaces

- **IR sensors** – Line following and directional turns
- **Proximity sensor** – Traffic block detection and parking triggers
- **Servo motor** – Steering control
- **DC motor (via Gizduino motor shield)** – Forward/reverse motion
- **RPi camera** – Color detection for traffic blocks and parking zones
- **IMU sensor** – Lap detection via yaw tracking
