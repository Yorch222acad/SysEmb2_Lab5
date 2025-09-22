import RPi.GPIO as GPIO
import serial
from time import sleep

PIN_BTN_UP = 6 
motorA = 13
motorA_1 = 19
motorA_2 = 26
estadom1 = False

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_BTN_UP, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(motorA, GPIO.OUT)
GPIO.setup(motorA_1, GPIO.OUT)
GPIO.setup(motorA_2, GPIO.OUT)
pwm = GPIO.PWM(motorA, 1000)
pwm.start(0)

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
            if value == "motor1":
                print("Recibido", value)
                if estadom1 == False:
                    pwm.ChangeDutyCycle(50)
                    GPIO.output(motorA_1, 1)
                    GPIO.output(motorA_2, 0)
                else:
                    pwm.ChangeDutyCycle(5)
                    GPIO.output(motorA_1, 1)
                    GPIO.output(motorA_2, 0)
            else:
                print("No data")
    except Exception as e:
        print(e)