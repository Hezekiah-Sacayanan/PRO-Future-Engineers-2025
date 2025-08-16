import cv2
import numpy as np
from picamera2 import Picamera2
import time
import serial

# --- Serial setup ---
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
time.sleep(2)

# --- Camera setup ---
picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'BGR888', "size": (640, 480)}))
picam2.set_controls({"AwbEnable": False, "ColourGains": (1.2, 1.8)})
picam2.start()
time.sleep(1)

# --- HSV color boundaries ---
# Red
lower_red1 = np.array([122, 255, 225])
upper_red1 = np.array([179, 255, 235])

# Green
lower_green = np.array([15, 190, 155])
upper_green = np.array([70, 210, 235])

# Purple
lower_purple = np.array([150, 255, 240])
upper_purple = np.array([179, 255, 255])

# --- Color detection function ---
def detect_color(hsv_frame):
    red_mask = cv2.inRange(hsv_frame, lower_red, upper_red)
    green_mask = cv2.inRange(hsv_frame, lower_green, upper_green)
    purple_mask = cv2.inRange(hsv_frame, lower_purple, upper_purple)

    red_pixels = cv2.countNonZero(red_mask)
    green_pixels = cv2.countNonZero(green_mask)
    purple_pixels = cv2.countNonZero(purple_mask)

    color_detected = "None"
    max_pixels = max(red_pixels, green_pixels, purple_pixels)
    threshold = 1000

    if max_pixels > threshold:
        if max_pixels == red_pixels:
            color_detected = "red"
        elif max_pixels == green_pixels:
            color_detected = "green"
        elif max_pixels == purple_pixels:
            color_detected = "purple"

    return color_detected

# --- Main loop ---
last_sent = ""

while True:
    frame = picam2.capture_array()
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    hsv_frame = cv2.cvtColor(frame_rgb, cv2.COLOR_RGB2HSV)

    color = detect_color(hsv_frame)

    if color != "None" and color != last_sent:
        if color == "red":
            ser.write(b'R')
            print("Sent: R")
            last_sent = "red"
        elif color == "green":
            ser.write(b'G')
            print("Sent: G")
            last_sent = "green"
        elif color == "purple":
            ser.write(b'P')
            print("Sent: P")
            last_sent = "purple"

    cv2.putText(frame_rgb, f"Detected: {color}", (10, 40),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

    key = cv2.waitKey(1) & 0xFF
    if key == ord("q"):
        break

cv2.destroyAllWindows()
