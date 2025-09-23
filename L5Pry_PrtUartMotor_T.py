import RPi.GPIO as GPIO
import serial
from time import sleep

BtnBuzzer = 6 
PwmMtrA = 13
HighMtrA = 19
LowMtrA = 26
estM1 = False

GPIO.setmode(GPIO.BCM)
GPIO.setup(BtnBuzzer, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(PwmMtrA, GPIO.OUT)
GPIO.setup(HighMtrA, GPIO.OUT)
GPIO.setup(LowMtrA, GPIO.OUT)
pwm = GPIO.PWM(PwmMtrA, 1000)
pwm.start(0)

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

while True:
    if GPIO.input(BtnBuzzer) == GPIO.LOW:
        ser.write(b"buzzer\n")
        print("enviado: buzzer")
        sleep(0.2)
    try:
        if ser.in_waiting > 0:
            value = ser.readline().decode('utf-8').rstrip()
            if value == "motor1":
                print("Recibido", value)
                if estM1 == False:
                    estM1 = True
                    pwm.ChangeDutyCycle(50)
                    GPIO.output(HighMtrA, 1)
                    GPIO.output(LowMtrA, 0)
                else:
                    estM1 = False
                    pwm.ChangeDutyCycle(0)
                    GPIO.output(HighMtrA, 1)
                    GPIO.output(LowMtrA, 0)
            else:
                print(value)
    except Exception as e:
        print(e)