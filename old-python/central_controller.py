from pid_controller import PID_Controller
import serial_controller as sc
from matplotlib import pyplot as plt
import subprocess

# r0, r1, r2 = math.radians(0), math.radians(120), math.radians(240)

# v0 = (math.cos(r0) * 0 - math.sin(r0) * 1, math.sin(r0) * 0 + math.cos(r0) * 1)
# v1 = (math.cos(r1) * 0 - math.sin(r1) * 1, math.sin(r1) * 0 + math.cos(r1) * 1)
# v2 = (math.cos(r2) * 0 - math.sin(r2) * 1, math.sin(r2) * 0 + math.cos(r2) * 1)
# plt.plot([v0[0], v1[0], v2[0]], [v0[1], v1[1], v2[1]])
# plt.show()

kp, ki, kd = 0.5, 0.000001, 7

servoTop = PID_Controller(kp, ki, kd)
servoLeft = PID_Controller(kp, ki, kd)
servoRight = PID_Controller(kp, ki, kd)

### sim
# simLength = 400
# servoTopAct = 0
# servoLeftAct = 0
# servoRightAct = 0
# speed = [0,0]
# distanceServos = 25.0
# servoTriHeight = math.sqrt(25.0**2 - (25.0/2.0)**2)

# r = []
# p = []
# x = []
# y = []
###

ballPosPerAxis = [0, 0, 0]
setpointPerAxis = [0, 0, 0]

cpp = subprocess.Popen("", executable="C:/Users/rick2/OneDrive/HU/MRB/libs/openFrameworks/apps/myApps/ball-tracking-app/bin/ball-tracking-app.exe", stdin=subprocess.PIPE, stdout=subprocess.PIPE, universal_newlines=True, bufsize=1)
while cpp.poll() == None:
    stringPos = cpp.stdout.readline().split()
    # print(stringPos)
    if len(stringPos) > 5:
        ballPosPerAxis[0] = float(stringPos[0])
        setpointPerAxis[0] = float(stringPos[1])
        ballPosPerAxis[1] = float(stringPos[2])
        setpointPerAxis[1] = float(stringPos[3])
        ballPosPerAxis[2] = float(stringPos[4])
        setpointPerAxis[2] = float(stringPos[5])
    # print(ballPosPerAxis)
    # print(setpointPerAxis)

### sim
# for i in range(0,simLength):
    # stH = math.cos(math.radians(servoTopAct + 45.0)) * 5.5
    # slH = math.cos(math.radians(servoLeftAct + 45.0)) * 5.5
    # srH = math.cos(math.radians(servoRightAct + 45.0)) * 5.5

    # roll = math.atan((slH-srH) / distanceServos)
    # pitch = math.atan((stH-((slH-srH)/2.0 + srH)) / servoTriHeight)
    # r.append(roll)
    # p.append(pitch)

    # speed[0] += roll
    # speed[1] -= pitch
    # pos = list( map(add, pos, speed) )
    # x.append(pos[0])
    # y.append(pos[1])

    #print(f"roll: {roll}, pitch: {pitch}, pos: {pos}")
    ###

    servoTopAct = servoTop.getAction(ballPosPerAxis[0], setpointPerAxis[0])
    servoLeftAct = servoLeft.getAction(ballPosPerAxis[1], setpointPerAxis[1])
    servoRightAct = servoRight.getAction(ballPosPerAxis[2], setpointPerAxis[2])

    sc.setAngle(servoTopAct + 45.0)
    sc.setAngle(servoLeftAct + 45.0)
    sc.setAngle(servoRightAct + 45.0)

servoTop.graph()
servoLeft.graph()
servoRight.graph()

###sim
# plt.figure()
# plt.plot(range(0,simLength), r, label='roll')
# plt.plot(range(0,simLength), p, label='pitch')
# plt.plot(range(0,simLength), [0]*simLength, label='0')
# plt.legend()

# plt.figure()
# # plt.axis('square')
# plt.plot(x, y, label='ball pos')
# # plt.gca().set_aspect('equal', adjustable='box')
# plt.legend()

# plt.show()
###
