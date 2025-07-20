#include <Servo.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 imu;
Servo steeringServo;

// --- Pin Assignments ---
const int IR_LEFT = A0;
const int IR_RIGHT = A1;
const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int PWM_PIN = 3;
const int DIR_PIN = 12;
const int SERVO_PIN = 9;
const int SWITCH_PIN = 10; // Tactile switch

// --- Thresholds & Settings ---
const int IR_THRESHOLD = 500;
const int PROX_THRESHOLD = 15; // cm
const int TARGET_LAPS = 3;
const float YAW_TOLERANCE = 25.0;
const int ALIGN_DISTANCE = 12; // cm before parking alignment

// --- State Variables ---
int lapCount = 0;
float yawRef = 0;
bool parkingReady = false;
bool started = false;
char blockColor = ' ';

// ---------------------------
// Setup
// ---------------------------
void setup() {
  Serial.begin(9600);
  Wire.begin();
  imu.initialize();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Tactile switch

  steeringServo.attach(SERVO_PIN);
  setSteeringCenter();
  setYawReference();
}

// ---------------------------
// Main Loop
// ---------------------------
void loop() {
  if (!started) {
    if (digitalRead(SWITCH_PIN) == LOW) {
      delay(200);  // Debounce
      started = true;
    }
    return;
  }

  updateIMU();
  checkLap();

  if (parkingReady && blockColor == 'P' && getDistance() < PROX_THRESHOLD) {
    performPreciseParking();
    return;
  }

  handleLineAndBlocks();
  delay(50);
}

// ---------------------------
// Movement & Control Functions
// ---------------------------

void moveForward(int speed) {
  digitalWrite(DIR_PIN, HIGH);
  analogWrite(PWM_PIN, speed);
}

void moveBackward(int speed) {
  digitalWrite(DIR_PIN, LOW);
  analogWrite(PWM_PIN, speed);
}

void stopMotors() {
  analogWrite(PWM_PIN, 0);
}

void setSteeringLeft() {
  steeringServo.write(0);
}

void setSteeringRight() {
  steeringServo.write(180);
}

void setSteeringCenter() {
  steeringServo.write(90);
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

// ---------------------------
// Parking Alignment and Execution
// ---------------------------
void performPreciseParking() {
  stopMotors();
  delay(300);

  // Step 1: Move forward until close to block
  setSteeringCenter();
  while (getDistance() > ALIGN_DISTANCE) {
    moveForward(150);
    delay(50);
  }
  stopMotors();
  delay(300);

  // Step 2: Get current yaw to decide side of alignment
  float currentYaw = imu.getAngleZ();
  bool preferLeft = currentYaw > yawRef;

  // Step 3: Slight turn to side for alignment
  if (preferLeft) {
    setSteeringLeft();
  } else {
    setSteeringRight();
  }
  moveForward(150);
  delay(400);
  stopMotors();
  delay(200);

  // Step 4: Opposite sharp turn and reverse
  if (preferLeft) {
    setSteeringRight();
  } else {
    setSteeringLeft();
  }
  moveBackward(150);
  delay(800);
  stopMotors();
  delay(200);

  // Step 5: Switch steering to other side and continue reversing
  if (preferLeft) {
    setSteeringLeft();
  } else {
    setSteeringRight();
  }
  moveBackward(150);
  delay(800);
  stopMotors();
  delay(200);

  // Step 6: Center steering and roll forward slightly
  setSteeringCenter();
  moveForward(150);
  delay(500);

  stopMotors(); // Done parking
}

// ---------------------------
// Sensor & Logic Functions
// ---------------------------
void updateIMU() {
  imu.getRotation();
}

void setYawReference() {
  imu.getRotation();
  yawRef = imu.getAngleZ();
}

void checkLap() {
  float yawNow = imu.getAngleZ();
  if (abs(yawNow - yawRef) < YAW_TOLERANCE) {
    lapCount++;
    setYawReference();
    delay(1000);
  }
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 20000);
  int distanceCm = duration * 0.034 / 2;
  return distanceCm;
}

void handleLineAndBlocks() {
  int left = analogRead(IR_LEFT);
  int right = analogRead(IR_RIGHT);
  int distance = getDistance();

  // Step beside magenta block before parking
  if (blockColor == 'P' && lapCount >= TARGET_LAPS && distance < PROX_THRESHOLD) {
    goStraight();
    delay(1000);
    stopMotors();
    parkingReady = true;
    return;
  }

  // Handle colored blocks
  if (distance < PROX_THRESHOLD) {
    if (blockColor == 'R') {
      turnRight();
    } else if (blockColor == 'G') {
      turnLeft();
    }
  } else {
    // Follow line using analog IR values
    if (left < IR_THRESHOLD && right >= IR_THRESHOLD) {
      turnLeft();
    } else if (right < IR_THRESHOLD && left >= IR_THRESHOLD) {
      turnRight();
    } else {
      goStraight();
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char input = Serial.read();
    if (input == 'R' || input == 'G' || input == 'P') {
      blockColor = input;
    }
  }
}
