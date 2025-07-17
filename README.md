# Engineering Materials

This repository contains engineering materials for a **self-driving vehicle prototype** designed for the **PRO Future Engineers competition – 2025**.

---

## Content

- **`t-photos/`** – Two team photos: one official and one fun photo featuring all team members  
- **`v-photos/`** – Six vehicle photos showing all angles (front, back, left, right, top, bottom)  
- **`video/`** – A `video.md` file containing a link to the demonstration video of the vehicle in action  
- **`schemes/`** – Schematic diagrams (JPEG, PNG, or PDF) of the electromechanical system, showing all key components (sensors, motors, controllers) and their connections  
- **`src/`** – Source code of the control software used in the vehicle, including modules for sensors, decision-making, and motor control  
- **`models/`** – CAD or fabrication files (e.g. STL, DXF) used for 3D printing, laser cutting, or CNC machining of mechanical parts *(remove if unused)*  
- **`other/`** – Supplementary documentation: SBC/SBM connection guides, datasets, hardware specs, or communication protocols *(remove if unused)*

---

## Introduction

This repository documents the **engineering, programming, and assembly** of a self-driving robot built for the WRO Future Engineers 2022 challenge.

### Code Architecture

The main control system is modular and consists of:

- **Sensor Modules:**
  - **IR Sensors (x2):** Detect diagonal blue/orange lines for directional turns
  - **Proximity Sensor:** Measures distance from nearby obstacles and traffic blocks
  - **RPi Camera Module:** Detects traffic block colors (green/red for direction, magenta for parking)
  - **IMU Sensor:** Tracks yaw orientation to detect laps and assist in parking alignment

- **Actuator Modules:**
  - **DC Motor (via Gizduino Motor Shield):** Controls forward/reverse motion with speed via `PWM` and direction via `DIR`
  - **Servo Motor:** Controls steering (left/right/center) based on sensor values

### Logic Flow

The vehicle uses a rule-based logic system that reacts in real-time to environmental inputs:
1. **IR sensor triggers** determine whether to turn left or right based on line detection
2. **Traffic block color** is identified by the RPi Camera
3. **Proximity sensor** signals when to begin turning around a block
4. **IMU sensor yaw** is used to detect lap completion
5. **Parallel parking** is triggered after 3 laps and detection of a magenta block

---

## Build & Upload Process

- Code is written in Arduino C++ and found in the `src/` directory
- The robot uses a **Gizduino Motor Shield** to control the DC motor:
  - `DIR` pin for direction (HIGH = forward, LOW = reverse)
  - `PWM` pin for speed (0–255)
- Servo control is handled via a dedicated PWM pin (e.g. D10)
- Upload code using the **Arduino IDE** to a **Gizduino-compatible board**
- The Raspberry Pi Camera operates independently and communicates detection results to the Arduino (via Serial or GPIO)

---

## Team Notes

This project was built collaboratively by a high school team to explore embedded systems, sensor fusion, and autonomous robotics. It represents a hands-on application of electronics, coding, and teamwork to solve real-world engineering challenges under competition constraints.

---

