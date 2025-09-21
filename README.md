# Engineering Materials

This repository contains the complete engineering documentation for a **self-driving vehicle prototype** developed for the **PRO Future Engineers competition – 2025**. This autonomous vehicle was designed, constructed, and programmed by a high school team as part of a hands-on robotics and embedded systems project. The repository serves as a comprehensive archive of all technical materials related to the robot's design, assembly, control software, sensor integration, and mechanical fabrication.

---

## Content

This repository is organized into several folders, each containing a specific set of project files and documentation:

- **`t-photos/`** – Includes two team photographs: one official image for competition use and one creative or humorous team photo that captures our spirit and teamwork.
  
- **`v-photos/`** – Contains six detailed photographs of the vehicle, showing every side: front, back, left, right, top, and bottom. These images are meant to give a complete view of the physical layout and mechanical build.

- **`video/`** – Contains a file named `video.md` that links to a demonstration video of the robot in motion. This video showcases the robot’s key capabilities including lap navigation, obstacle avoidance, and final parking.

- **`schemes/`** – Includes electromechanical schematics (in PNG, JPEG, or PDF format). These diagrams illustrate the complete wiring setup including sensors, motors, controllers, and power management.

- **`src/`** – Contains all source code required to operate the vehicle. The code is written in Arduino C++ and includes modular functions for sensor reading, motor control, decision logic, lap detection, and more.

- **`models/`** – Includes any 3D models (e.g., `.stl`, `.dxf`) used to fabricate physical parts of the robot. These files are compatible with 3D printing, CNC machining, or laser cutting. *(This folder can be removed if no files are provided.)*

- **`other/`** – Supplementary documentation that may include pin mapping diagrams, communication protocol descriptions, hardware specifications, and other engineering references used during the development of the project.

---

## Introduction

This project was developed as part of the WRO/PRO Future Engineers 2025 challenge, which tasked student teams with designing and building an autonomous ground vehicle capable of navigating a defined arena, identifying obstacles, completing laps, and performing an automated parking task.

Our solution uses a combination of **Arduino (Gizduino-compatible) hardware**, **sensors**, **servo and DC motors**, and a **Raspberry Pi camera** to detect environmental cues and make real-time movement decisions. The vehicle follows a modular code structure with clearly defined logic, making it reliable and maintainable throughout testing and competition.

---

## Code Architecture

The code is designed around modular blocks that handle specific hardware components and decision logic. Below is an overview of the core modules and their function:

### Sensor Modules

- **Color Sensor**: Mounted underneath the vehicle to detect changes in surface color, specifically, transitions between diagonal blue and orange lines. The detection of these lines allows the robot to count how many corners it has passed, thus counting how many laps it has done.
  
- **Proximity Sensor**: Used to measure the distance between the robot and nearby traffic blocks. This helps determine when to initiate turns and avoid collisions with obstacles.

- **Raspberry Pi Camera Module**: Positioned front-facing and programmed to detect the color of traffic blocks. Green blocks signal left turns, red blocks signal right turns, and magenta blocks mark the parking zone.

### Actuator Modules

- **DC Motor (Gizduino Motor Shield)**: Drives the wheels forward or backward. Motor direction is controlled using a digital DIR pin, and speed is controlled using a PWM signal. The Gizduino motor shield simplifies connection and control of brushed DC motors.

- **Servo Motor**: Attached to the steering mechanism. It turns the front wheels left, right, or keeps them centered based on movement instructions derived from sensor inputs.

---

## Behavior Logic

The vehicle operates using a rule-based system structured around sensor input and behavioral state transitions. Here's an overview of the flow:

1. **Startup**: The robot initializes all sensors and motors, sets yaw reference from IMU, and waits for the official start command.

2. **Line Detection and Lap Counting**: As it begins moving forward, the color sensor monitors the arena surface. As the sensor detects a color change, the value of the robot's line counter increases by 1. Once the line counter has reached a certain value, the robot counts that run as a whole lap.

3. **Color Recognition and Turning**: The Raspberry Pi camera captures the color of traffic blocks in front of the robot. If a green block is seen, the robot prepares to turn left. If a red block is seen, it prepares to turn right. These decisions override IR sensor input momentarily when a traffic block is close.

4. **Proximity Detection**: Once a traffic block is confirmed, the proximity sensor measures distance. When a threshold distance is reached (indicating closeness), the vehicle slows and initiates a turn around the block.

5. **Parallel Parking**: Upon completing the third lap, the robot waits until it detects a magenta block. It then begins the parallel parking maneuver using a pre-programmed sequence of steering and motor movements to slide into the designated area.

---

## Build & Upload Process

1. **Connect Components**:
   - Color sensor → digital pins on Gizduino
   - Proximity sensor → analog pin
   - Servo → PWM pin (e.g., D10)
   - DC motors → connected to the Gizduino motor shield (Motor A terminals)
   - Raspberry Pi camera → attached and configured via Pi software, communicates via Serial or GPIO

2. **Upload Code**:
   - Open `src/main.ino` in Arduino IDE
   - Select the appropriate board (e.g., Gizduino UNO or ATmega328p)
   - Connect the board and upload the code via USB

3. **Test Communication**:
   - Confirm sensor readings via `Serial Monitor`
   - Verify servo and motor movement based on simple condition triggers

4. **Final Calibration**:
   - Adjust color sensor thresholds, motor speed values, and servo angles.

---

## Team Notes

This robot was developed by a group of high school students passionate about robotics, mechatronics, and computer engineering. Through this project, the team gained experience in modular design, sensor fusion, motor control, and embedded programming. Every challenge, from designing steering mechanics to refining lap detection, helped us grow as engineers and teammates.

We hope that this repository serves not only as documentation for the competition, but also as a useful reference for other teams and students interested in building autonomous vehicles using affordable and accessible tools like Arduino, Gizduino, and Raspberry Pi.

---

## Acknowledgments

We would like to thank our mentors, coaches, and teammates who supported the project through testing, debugging, and constant feedback. We are also grateful for the opportunity to participate in the PRO Future Engineers 2025 challenge, which encouraged us to push the boundaries of what we could achieve as student innovators.

---
