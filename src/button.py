import RPi.GPIO as GPIO
import time
import subprocess
import os
import signal

BUTTON_PIN = 17  # GPIO17 = pin 11
SCRIPT_PATH = "/home/pi/color.py"

GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# --- State ---
script_process = None
script_running = False

# --- Kill any process using the camera ---
def kill_camera_users():
    try:
        result = subprocess.check_output(["lsof", "-t", "/dev/video0"])
        for pid in result.decode().strip().split('\n'):
            if pid.strip().isdigit():
                subprocess.run(["kill", "-9", pid])
        print("Camera processes terminated.")
    except subprocess.CalledProcessError:
        print("No camera process found.")

print("Waiting for button press...")

try:
    while True:
        if GPIO.input(BUTTON_PIN) == GPIO.LOW:
            if not script_running:
                print("Button pressed — starting script...")
                kill_camera_users()
                script_process = subprocess.Popen(["python3", SCRIPT_PATH])
                script_running = True
            else:
                print("Button pressed — stopping script...")
                if script_process and script_process.poll() is None:
                    script_process.terminate()
                    time.sleep(0.5)
                    if script_process.poll() is None:
                        script_process.kill()
                    print("Script terminated.")
                script_running = False

            # Debounce wait until button is released
            while GPIO.input(BUTTON_PIN) == GPIO.LOW:
                time.sleep(0.05)

        time.sleep(0.1)

except KeyboardInterrupt:
    print("Exiting...")

finally:
    if script_process and script_process.poll() is None:
        script_process.kill()
    GPIO.cleanup()
