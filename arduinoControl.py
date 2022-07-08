import serial as ser
import time

ser = ser.Serial('/dev/ttyACM0', 115200)
ser.open()

while True:
    input = input('Enter your selection')
    ser.write(input)

