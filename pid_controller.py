from time import time
from matplotlib import pyplot as plt

class PID_Controller:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd

        self.prevError = 0
        self.iError = 0
    
        self.prevTime = time()

        ### DEBUG
        self.out = []
        self.errors = []
        self.poss = []
        self.iErrors = []
        ###

    def getAction(self, pos, setpoint):
        error = setpoint - pos
        self.iError += error
        self.iErrors.append(self.iError * self.ki)
        action = self.kp * error + self.ki * self.iError + self.kd * ((error - self.prevError))# / (time() - prevTime)) time interval too short, causes div by 0 error
        action = max(action, -10.0) # Clip to
        action = min(action, 45.0) # servo range
        self.prevTime = time()
        self.prevError = error

        self.errors.append(error/10.0)
        self.out.append(action)
        self.poss.append(pos/10.0)

        return action

    ### DEBUG
    def graph(self):
        plt.figure()
        plt.plot(range(0,len(self.out)), self.out, label='action')
        plt.plot(range(0,len(self.out)), self.errors, label='error/10.0')
        plt.plot(range(0,len(self.out)), self.poss, label='position/10.0')
        plt.plot(range(0,len(self.out)), self.iErrors, label='i error * ki')
        plt.plot(range(0,len(self.out)), [0]*len(self.out), label='0')
        plt.legend()
        # plt.show()
    ###