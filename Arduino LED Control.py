import serial # Library which allows me to talk to the arduino via a serial port 
import time

while True:
    with serial.Serial('/dev/ttyACM0', 9600,) as a: # Here the /dev/ttyACM0 is the serial port my arduino is connected to, this will look different on Windows as there is a slightly different method of naming the ports compared to one used on my linux machine
        a.write(b'H')# send H (the high signal) to the serial port
        time.sleep(0.5)# pause
        a.write(b'L')# send L (the low signal) to the serial port
        time.sleep(0.5) # pause
        
        
