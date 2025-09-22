import RPi.GPIO as GPIO
import serial
from time import sleep

PIN_BTN_UP = 6 
GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_BTN_UP, GPIO.IN, pull_up_down=GPIO.PUD_UP)

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

while True:
    if GPIO.input(PIN_BTN_UP) == GPIO.LOW:
        ser.write(b"buzzer\n")
        print("enviado: buzzer")
        sleep(0.1)
    try:
        if ser.in_waiting > 0:
            value = ser.readline().decode('utf-8').rstrip()
            if value:
                print("Recibido", value)
            else:
                print("No data")
    except Exception as e:
        print(e)