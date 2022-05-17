from time import sleep
import serial
import math

if __name__ == '__main__':
    ser = serial.Serial('COM4', 57600, timeout=1)
    ser.reset_input_buffer()
    
    sleepTime = 1
    
    while True:
        # for pos in range(135):
        #     ser.write(bytes(str(pos) + "\n", 'utf-8'))
        #     # line = ser.readline().decode('utf-8').rstrip()
        #     # print(line)
        #     sleep(sleepTime)

        # for pos in range(135, 0, -1):
        #     ser.write(bytes(str(pos) + "\n", 'utf-8'))
        #     # line = ser.readline().decode('utf-8').rstrip()
        #     # print(line)
        #     sleep(sleepTime)
    
        angle = input("Enter new angle 1: ")
        ser.write(bytes(str(angle) + "\n", 'utf-8'))

        height = math.cos(math.radians(float(angle))) * 50.0

        print(f"Height: {height}")
        