from time import sleep
import serial

if __name__ == '__main__':
    ser = serial.Serial('COM4', 57600, timeout=1)
    ser.reset_input_buffer()
    
    sleepTime = 0.0001
    
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
    
        ser.write(bytes(str(0) + "\n", 'utf-8'))
        sleep(2.5)
        ser.write(bytes(str(125) + "\n", 'utf-8'))
        sleep(2.5)