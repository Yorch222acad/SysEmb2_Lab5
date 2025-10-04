# Librerías:
import RPi.GPIO as GPIO # GPIO
import serial # Comunicación Serial
from time import sleep # Retardo

# Variables:
BtnBuzzer = 5 # Botón Buzzer
#----------------------{ Motores PWM
PwmMtrB = 6 
PwmMtrA = 26
HighMtrAB = 13 
LowMtrAB = 19
estM1 = estM2 = False
#}--

# Configuración:
GPIO.setmode(GPIO.BCM) # Pines (Modo de numeración BCM)
GPIO.setup(BtnBuzzer, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Buzzer
#--------------------------------{ Motores PWM
GPIO.setup(PwmMtrA, GPIO.OUT)
GPIO.setup(PwmMtrB, GPIO.OUT)
GPIO.setup(HighMtrAB, GPIO.OUT)
GPIO.setup(LowMtrAB, GPIO.OUT)

# Configuración PWM: 
pwmA = GPIO.PWM(PwmMtrA, 1000)
pwmB = GPIO.PWM(PwmMtrB, 1000)
pwmA.start(0)
pwmB.start(0)

# Configuración UART:
ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

# Inicialización:
GPIO.output(HighMtrAB, 1)
GPIO.output(LowMtrAB, 0)

while True:
    try:
        # Al presionar el botón del Buzzer envía el comando "buzzer" por UART: {=>
        if GPIO.input(BtnBuzzer) == GPIO.LOW:
            ser.write(b"buzzer\n")
            print("enviado: buzzer")
            sleep(0.2)
        # <=} Si recibe datos por UART:{=>
        if ser.in_waiting > 0:
            value = ser.readline().decode('utf-8').rstrip()
            # Si "motor1" activar - desactivar Motor 1 [=>
            if value == "motor1":
                print("Recibido1", value)
                if estM1 == False:
                    estM1 = True
                    pwmA.ChangeDutyCycle(50)
                else:
                    estM1 = False
                    pwmA.ChangeDutyCycle(0)
            # <=] Si "motor1" activar - desactivar Motor 1 [=>
            if value == "motor2":
                print("Recibido2", value)
                if estM2 == False:
                    estM2 = True
                    pwmB.ChangeDutyCycle(50)
                else:
                    estM2 = False
                    pwmB.ChangeDutyCycle(0)
            # <=]
            else:
                print(value)
        # <=}
    except Exception as e:
        print(e)
