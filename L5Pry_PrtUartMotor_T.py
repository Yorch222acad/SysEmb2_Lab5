import RPi.GPIO as GPIO
import serial
from time import sleep

BtnBuzzer = 5
PwmMtrB = 6 
PwmMtrA = 26
HighMtrAB = 13 
LowMtrAB = 19
estM1 = estM2 = False

GPIO.setmode(GPIO.BCM)
GPIO.setup(BtnBuzzer, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(PwmMtrA, GPIO.OUT)
GPIO.setup(PwmMtrB, GPIO.OUT)
GPIO.setup(HighMtrAB, GPIO.OUT)
GPIO.setup(LowMtrAB, GPIO.OUT)
pwmA = GPIO.PWM(PwmMtrA, 1000)
pwmB = GPIO.PWM(PwmMtrB, 1000)
pwmA.start(0)
pwmB.start(0)

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

GPIO.output(HighMtrAB, 1)
GPIO.output(LowMtrAB, 0)

while True:
    try:
        if GPIO.input(BtnBuzzer) == GPIO.LOW:
            ser.write(b"buzzer\n")
            print("enviado: buzzer")
            sleep(0.2)
        if ser.in_waiting > 0:
            value = ser.readline().decode('utf-8').rstrip()
            if value == "motor1":
                print("Recibido1", value)
                if estM1 == False:
                    estM1 = True
                    pwmA.ChangeDutyCycle(50)
                else:
                    estM1 = False
                    pwmA.ChangeDutyCycle(0)
            if value == "motor2":
                print("Recibido2", value)
                if estM2 == False:
                    estM2 = True
                    pwmB.ChangeDutyCycle(50)
                else:
                    estM2 = False
                    pwmB.ChangeDutyCycle(0)
            else:
                print(value)
    except Exception as e:
        print(e)
