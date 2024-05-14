import time

import serial

# Change 'COMX' to your Arduino's port and '9600' to the baud rate
ser = serial.Serial('COM5', 9600)

file = open("arduino_data.txt", "a")  # Open or create a file named arduino_data.txt for appending data

while True:
    if ser.in_waiting > 0:
        data = ser.readline().decode().strip()  # Read data from the serial port
        file.write(data + "\n")                 # Write data to the file
    time.sleep(0.5)