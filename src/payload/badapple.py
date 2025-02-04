import cv2
import serial
import time
import numpy as np

ser = serial.Serial('COM3', 115200, timeout=1)
time.sleep(2)

video = cv2.VideoCapture('badapple.mp4')

if not video.isOpened():
    print("Error while loading.")
    exit()

while True:
    ret, frame = video.read()
    if not ret:
        break

    while ser.read() != b'K':
        pass

    resized = cv2.resize(frame, (15, 16), interpolation=cv2.INTER_LINEAR)
    
    gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
    
    _, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)
    
    binary_array = (binary // 255).astype(np.uint8)
    binary_list = binary_array.flatten().tolist()
    
    try:
        ser.write(bytes(binary_list))
    except Exception as e:
        print(f"Error: {e}")

video.release()
ser.close()