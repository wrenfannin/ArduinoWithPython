import serial as ser # imports the PySerial library

ser = ser.Serial('/dev/ttyACM0', 115200) # Selects the serial port /dev/tty/ttyACM0 at a baud rate of 115200
ser.open() # Opens the port /dev/ttyACM0

while True: # Start a forever loop
    input = input('Enter your selection') # Ask for input to the python shell
    ser.write(input) # Sends the input to the arduino

