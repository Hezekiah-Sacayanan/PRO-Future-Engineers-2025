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
const int START_BUTTON_PIN = 10;

// --- Thresholds & Settings ---
const int IR_THRESHOLD = 500;        // For analog IR detection
const int PROX_THRESHOLD = 15;       // cm
const int TARGET_LAPS = 3;
const float YAW_TOLERANCE = 25.0;    // degrees

// --- State Variables ---
int lapCount = 0;
float yawRef = 0;
bool parkingReady = false;
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
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);

  steeringServo.attach(SERVO_PIN);
  setSteeringCenter();

  waitForStart();
  setYawReference();
}

// ---------------------------
// Wait for Start Button
// ---------------------------
void waitForStart() {
  Serial.println("Waiting for START button...");
  while (digitalRead(START_BUTTON_PIN) == HIGH) {
    delay(10); // Wait until button is pressed (active LOW)
  }
  Serial.println("Started!");
}

// ---------------------------
// Main Loop
// ---------------------------
void loop() {
  updateIMU();
  checkLap();

  if (parkingReady && blockColor == 'P' && getDistance() < PROX_THRESHOLD) {
    performParallelParking();
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
  steeringServo.write(0);  // full left
}

void setSteeringRight() {
  steeringServo.write(180); // full right
}

void setSteeringCenter() {
  steeringServo.write(90);  // center
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
// Parking Function
// ---------------------------
void performParallelParking() {
  // STEP 2: Turn right and reverse into angle
  setSteeringRight();
  moveBackward(150);
  delay(900);

  // STEP 3: Straighten wheels and reverse
  setSteeringCenter();
  delay(100);
  moveBackward(150);
  delay(900);

  // STEP 4: Turn left and finish reversing
  stopMotors();
  delay(200);
  setSteeringLeft();
  delay(200);
  moveBackward(150);
  delay(900);

  // STEP 5: Straighten and move forward slightly to center
  stopMotors();
  delay(200);
  setSteeringCenter();
  moveForward(150);
  delay(500);

  stopMotors(); // You're parked!
}

// ---------------------------
// Sensor & Logic Functions
// ---------------------------

void updateIMU() {
  imu.getRotation();  // Used for potential side effects
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

  // Step beside purple block before parking
  if (blockColor == 'P' && lapCount >= TARGET_LAPS && distance < PROX_THRESHOLD) {
    goStraight();
    delay(1000); // Move forward beside block
    stopMotors();
    parkingReady = true;
    return;
  }

  if (distance < PROX_THRESHOLD) {
    if (blockColor == 'R') {
      turnRight();
    } else if (blockColor == 'G') {
      turnLeft();
    }
  } else {
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
