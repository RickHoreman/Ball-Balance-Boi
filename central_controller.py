from pid_controller import PID_Controller
import math
from matplotlib import pyplot as plt

r0, r1, r2 = math.radians(0), math.radians(120), math.radians(240)

v0 = (math.cos(r0) * 0 - math.sin(r0) * 1, math.sin(r0) * 0 + math.cos(r0) * 1)
v1 = (math.cos(r1) * 0 - math.sin(r1) * 1, math.sin(r1) * 0 + math.cos(r1) * 1)
v2 = (math.cos(r2) * 0 - math.sin(r2) * 1, math.sin(r2) * 0 + math.cos(r2) * 1)
# plt.plot([v0[0], v1[0], v2[0]], [v0[1], v1[1], v2[1]])
# plt.show()

servoTop = PID_Controller(10, 0.1, 1, v0)
servoLeft = PID_Controller(10, 0.1, 1, v1)
servoRight = PID_Controller(10, 0.1, 1, v2)

servoTopAct = 0
servoLeftAct = 0
servoRightAct = 0

pos = [0, 0]
setpoint = [0, 1]
for i in range(0,100):
    servoTopAct = servoTop.getAction(pos, setpoint)
    servoLeftAct = servoLeft.getAction(pos, setpoint)
    servoRightAct = servoRight.getAction(pos, setpoint)
    pos[1] += 0.1

servoTop.graph()
servoLeft.graph()
servoRight.graph()
plt.show()
