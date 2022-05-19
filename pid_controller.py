from time import time
from matplotlib import pyplot as plt

class PID_Controller:
    def __init__(self, kp, ki, kd, angleVector):
        """kp, ki, and kd are the p, i, and d scalars if the pid controller and should be some float,
        angleVector should be a tuple or list of length 2 corresponding to some vector of amplitude 1 pointing in the same direction as the servo in the physical setup relative to the xy position provided by the camera tracking module."""
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.angleVector = angleVector

        self.prevError = 0
        self.iError = 0
    
        self.prevTime = time()

        ### DEBUG
        self.out = []
        self.errors = []
        self.poss = []
        self.testSize=100
        ###

    def getAction(self, pos, setpoint):
        error = sum([a * b for a, b in zip(setpoint, self.angleVector)]) - sum([a * b for a, b in zip(pos, self.angleVector)])
        self.iError += error
        action = self.kp * error + self.ki * self.iError + self.kd * ((error - self.prevError))# / (time() - prevTime)) time interval too short, causes div by 0 error
        action = max(action, -45.0) # Clip to
        action = min(action, 45.0) # servo range
        self.prevTime = time()
        self.prevError = error

        self.errors.append(error)
        self.out.append(action)
        self.poss.append(sum([a * b for a, b in zip(pos, self.angleVector)]))

        return action

    ### DEBUG
    def graph(self):
        plt.figure()
        plt.plot(range(0,self.testSize), self.out, label='action')
        plt.plot(range(0,self.testSize), self.errors, label='error')
        plt.plot(range(0,self.testSize), self.poss, label='position')
        plt.plot(range(0,self.testSize), [0]*self.testSize, label='0')
        plt.legend()
        # plt.show()
    ###