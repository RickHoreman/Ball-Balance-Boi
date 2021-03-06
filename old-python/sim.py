from pid_controller import PID_Controller
from matplotlib import pyplot as plt
import math
from operator import add

### Make transformation matrices
r0, r1, r2 = math.radians(0), math.radians(120), math.radians(240)

v0 = (math.cos(r0) * 0 - math.sin(r0) * 1, math.sin(r0) * 0 + math.cos(r0) * 1)
v1 = (math.cos(r1) * 0 - math.sin(r1) * 1, math.sin(r1) * 0 + math.cos(r1) * 1)
v2 = (math.cos(r2) * 0 - math.sin(r2) * 1, math.sin(r2) * 0 + math.cos(r2) * 1)
# plt.plot([v0[0], v1[0], v2[0]], [v0[1], v1[1], v2[1]])
# plt.show()
###

kp, ki, kd = 0.1, 0.000001, 7

servoTop = PID_Controller(kp, ki, kd)
servoLeft = PID_Controller(kp, ki, kd)
servoRight = PID_Controller(kp, ki, kd)

simLength = 400
servoTopAct = 0
servoLeftAct = 0
servoRightAct = 0
speed = [0,0]
distanceServos = 25.0
servoTriHeight = math.sqrt(25.0**2 - (25.0/2.0)**2)

r = []
p = []
x = []
y = []

ballPos = [0,0]
setpoint = [400, 400]

for i in range(0,simLength):
    stH = math.cos(math.radians(servoTopAct + 45.0)) * 5.5
    slH = math.cos(math.radians(servoLeftAct + 45.0)) * 5.5
    srH = math.cos(math.radians(servoRightAct + 45.0)) * 5.5

    roll = math.atan((slH-srH) / distanceServos)
    pitch = math.atan((stH-((slH-srH)/2.0 + srH)) / servoTriHeight)
    r.append(roll)
    p.append(pitch)

    speed[0] += roll
    speed[1] -= pitch
    ballPos = list( map(add, ballPos, speed) )
    x.append(ballPos[0])
    y.append(ballPos[1])

    print(f"roll: {roll}, pitch: {pitch}, ballPos: {ballPos}")
    servoTopAct = servoTop.getAction(sum([a * b for a, b in zip(ballPos, v0)]) , sum([a * b for a, b in zip(setpoint, v0)]))
    servoLeftAct = servoLeft.getAction(sum([a * b for a, b in zip(ballPos, v1)]) , sum([a * b for a, b in zip(setpoint, v1)]))
    servoRightAct = servoRight.getAction(sum([a * b for a, b in zip(ballPos, v2)]) , sum([a * b for a, b in zip(setpoint, v2)]))

servoTop.graph() # Figure 1
servoLeft.graph() # Figure 2
servoRight.graph() # Figure 3

### Figure 4
plt.figure()
plt.plot(range(0,simLength), r, label='roll')
plt.plot(range(0,simLength), p, label='pitch')
plt.plot(range(0,simLength), [0]*simLength, label='0')
plt.legend()
###

### Figure 5
plt.figure()
# plt.axis('square')
plt.plot(x, y, label='ball pos')
# plt.gca().set_aspect('equal', adjustable='box')
plt.legend()
### Figure

plt.show()
