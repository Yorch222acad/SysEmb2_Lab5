import RPi.GPIO as GPIO
from time import sleep
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

motorA = 13
motorA_1 = 19
motorA_2 = 26

GPIO.setup(motorA, GPIO.OUT)
GPIO.setup(motorA_1, GPIO.OUT)
GPIO.setup(motorA_2, GPIO.OUT)
pwm = GPIO.PWM(motorA, 1000)
pwm.start(0)
try:
    while True:
        for x in range(0,100):
            pwm.ChangeDutyCycle(x)
            GPIO.output(motorA_1, 1)
            GPIO.output(motorA_2, 0)
            sleep(0.01)
        sleep(1)
        for x in range(100,0,-1):
            pwm.ChangeDutyCycle(x)
            GPIO.output(motorA_1, 1)
            GPIO.output(motorA_2, 0)
            sleep(0.01)
        sleep(1)
except KeyboardInterrupt:
    pwm.stop()
    GPIO.cleanup()