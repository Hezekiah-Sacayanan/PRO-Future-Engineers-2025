#include <Servo.h>       // Library to control the servo motor
#include <Wire.h>        // Library for I2C communication
#include <MPU6050.h>     // Library to interface with MPU6050 IMU sensor

MPU6050 imu;             // Create IMU object
Servo steeringServo;     // Create Servo object for steering

// -------------------------------
// --- Pin Assignments -----------
// -------------------------------
const int IR_LEFT = A0;     // Analog pin for left IR sensor
const int IR_RIGHT = A1;    // Analog pin for right IR sensor
const int TRIG_PIN = 7;     // Ultrasonic sensor trigger
const int ECHO_PIN = 6;     // Ultrasonic sensor echo
const int PWM_PIN = 3;      // Motor PWM (speed control)
const int DIR_PIN = 12;     // Motor direction control
const int SERVO_PIN = 9;    // Servo signal pin
const int BUTTON_PIN = 10;  // Tactile button pin

// -------------------------------
// --- Thresholds & Settings -----
// -------------------------------
const int IR_THRESHOLD = 500;        // Analog IR threshold value
const int PROX_THRESHOLD = 15;       // Distance in cm to detect a block
const int TARGET_LAPS = 3;           // How many laps before parking
const float YAW_TOLERANCE = 25.0;    // Acceptable difference in yaw to detect lap completion

// -------------------------------
// --- State Variables -----------
// -------------------------------
int lapCount = 0;           // Counter for completed laps
float yawRef = 0;           // Reference yaw angle (initial)
bool parkingReady = false;  // Flag to begin parking routine
char blockColor = ' ';      // Current color seen by RPi ('R', 'G', 'P')
bool started = false;       // Start program only when button is pressed

// -------------------------------
// --- Setup ---------------------
// -------------------------------
void setup() {
  Serial.begin(9600);       // Start serial communication for debugging
  Wire.begin();             // Initialize I2C for MPU6050
  imu.initialize();         // Initialize IMU sensor

  // Setup I/O pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor

  // Initialize servo
  steeringServo.attach(SERVO_PIN);
  setSteeringCenter();      // Start with straight wheels
  setYawReference();        // Set starting yaw angle
}

// -------------------------------
// --- Main Loop -----------------
// -------------------------------
void loop() {
  // Wait for tactile button press to begin
  if (!started) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      started = true;
      delay(300); // Debounce delay
    } else {
      return;     // Skip rest of loop
    }
  }

  // Continuously update sensors
  updateIMU();
  checkLap();

  // When ready to park, execute alignment + parking
  if (parkingReady && blockColor == 'P' && getDistance() < PROX_THRESHOLD) {
    alignAndPark();
    return;
  }

  // Line following and block-based turning
  handleLineAndBlocks();
  delay(50); // Short delay for smoother control
}

// -------------------------------
// --- Movement Functions --------
// -------------------------------

void moveForward(int speed) {
  digitalWrite(DIR_PIN, HIGH);      // Set forward direction
  analogWrite(PWM_PIN, speed);      // Set motor speed
}

void moveBackward(int speed) {
  digitalWrite(DIR_PIN, LOW);       // Set backward direction
  analogWrite(PWM_PIN, speed);      // Set motor speed
}

void stopMotors() {
  analogWrite(PWM_PIN, 0);          // Stop motor
}

void setSteeringLeft() {
  steeringServo.write(0);           // Turn wheels fully left
}

void setSteeringRight() {
  steeringServo.write(180);         // Turn wheels fully right
}

void setSteeringCenter() {
  steeringServo.write(90);          // Center the wheels
}

void turnLeft() {
  setSteeringLeft();
  moveForward(180);
}

void turnRight() {
  setSteeringRight();
  moveForward(180);
}

void goStraight() {
  setSteeringCenter();
  moveForward(250);
}

// -------------------------------
// --- Align and Park ------------
// -------------------------------
void alignAndPark() {
  float yaw = imu.getAngleZ();        // Get current yaw angle
  float distance = getDistance();     // Get distance from ultrasonic
  float angleRad = radians(abs(yaw)); // Convert yaw to radians

  // Calculate how far to move forward to align parallel
  float alignDistance = distance * tan(angleRad);

  // Estimate time to move based on speed
  float forwardSpeed = 150.0;
  float estimatedSpeedCmPerSec = 5.0;
  int forwardTime = (alignDistance / estimatedSpeedCmPerSec) * 1000;

  Serial.print("Align distance: ");
  Serial.print(alignDistance);
  Serial.print(" cm, Time: ");
  Serial.println(forwardTime);

  // Step 1: Move forward to align with side of block
  setSteeringCenter();
  moveForward(forwardSpeed);
  delay(forwardTime);
  stopMotors();
  delay(300);

  // Step 2: Begin parking maneuver - diagonal reverse
  if (yaw < 0) setSteeringRight();   // Adjust turn based on previous orientation
  else setSteeringLeft();

  moveBackward(150);
  delay(900);
  stopMotors();
  delay(200);

  // Step 3: Straighten and continue reversing
  setSteeringCenter();
  moveBackward(150);
  delay(900);
  stopMotors();
  delay(200);

  // Step 4: Final curve into slot
  if (yaw < 0) setSteeringLeft();
  else setSteeringRight();

  moveBackward(150);
  delay(900);
  stopMotors();
  delay(200);

  // Step 5: Center and inch forward until magenta block is in front again
  setSteeringCenter();
  moveForward(150);
  while (getDistance() > 5) {
    delay(50);
  }
  stopMotors();
  Serial.println("Parking complete.");
}

// -------------------------------
// --- Sensor & Logic Functions ---
// -------------------------------
void updateIMU() {
  imu.getRotation();   // Updates internal state of MPU6050
}

void setYawReference() {
  imu.getRotation();
  yawRef = imu.getAngleZ(); // Save yaw angle for lap comparison
}

void checkLap() {
  float yawNow = imu.getAngleZ();
  if (abs(yawNow - yawRef) < YAW_TOLERANCE) {
    lapCount++;
    setYawReference();
    delay(1000); // Avoid multiple counts from small shakes
  }
}

// Get distance using ultrasonic sensor
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000); // Timeout after 20ms
  int distanceCm = duration * 0.034 / 2;          // Convert time to cm
  return distanceCm;
}

// -------------------------------
// --- Line + Block Handling -----
// -------------------------------
void handleLineAndBlocks() {
  int left = analogRead(IR_LEFT);
  int right = analogRead(IR_RIGHT);
  int distance = getDistance();

  // Detect magenta and prep for parking
  if (blockColor == 'P' && lapCount >= TARGET_LAPS && distance < PROX_THRESHOLD) {
    goStraight();
    delay(1000);
    stopMotors();
    parkingReady = true;
    return;
  }

  // Block detection logic (from RPi)
  if (distance < PROX_THRESHOLD) {
    if (blockColor == 'R') turnRight();
    else if (blockColor == 'G') turnLeft();
  } else {
    // Analog line following logic
    if (left < IR_THRESHOLD && right >= IR_THRESHOLD) turnLeft();
    else if (right < IR_THRESHOLD && left >= IR_THRESHOLD) turnRight();
    else goStraight();
  }
}

// -------------------------------
// --- RPi Communication ---------
// -------------------------------
void serialEvent() {
  while (Serial.available()) {
    char input = Serial.read(); // Expected: 'R', 'G', 'P'
    if (input == 'R' || input == 'G' || input == 'P') {
      blockColor = input;
    }
  }
}
