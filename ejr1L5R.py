import serial
from time import sleep

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.reset_input_buffer()

while True:
    try:
        ser.write(b"hola\n")   # Ahora sí manda al puerto serial
        print("enviado: hola")
        sleep(1.5)
    except Exception as e:
        print(e)