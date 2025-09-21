import serial
from time import sleep

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

while True:
    try:
        if ser.in_waiting > 0:
            value = ser.readline().decode('utf-8').rstrip()
            if value:
                print("RX:", value)
            else:
                print("No data")
    except Exception as e:
        print(e)