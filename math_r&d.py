import math
from operator import add
from re import S
from matplotlib import pyplot as plt
import random
random.seed(0)

# rT, rL, rR = math.radians(0), math.radians(120), math.radians(240)

# sum([a * b for a, b in zip(setpoint, self.angleVector)])

camRes = (640, 480)
calPoints = [(300, 20), (50, 470), (600, 460)]

triHeight = math.sqrt(250**2 - 125**2)
center = ((calPoints[0][0] + calPoints[1][0] + calPoints[2][0])/3.0, (calPoints[0][1] + calPoints[1][1] + calPoints[2][1])/3.0)
center2 = ((125 + 0 + 250)/3.0, (0 + triHeight + triHeight)/3.0)
print(center2)

s = math.sqrt((125-center2[0])**2 + (0-center2[1])**2)
vT = ((calPoints[0][0] - center[0]), (calPoints[0][1] - center[1]))
vL = ((calPoints[1][0] - center[0]), (calPoints[1][1] - center[1]))
vR = ((calPoints[2][0] - center[0]), (calPoints[2][1] - center[1]))
mT = math.sqrt(vT[0]**2 + vT[1]**2)
mL = math.sqrt(vL[0]**2 + vL[1]**2)
mR = math.sqrt(vR[0]**2 + vR[1]**2)
# print(f"{mT} {mL} {mR}")
print(f"T {vT} {mT} L {vL} {mL} R {vR} {mR}")
vT = (-vT[0] / mT * (s / mT), -vT[1] / mT * (s / mT))
vL = (-vL[0] / mL * (s / mL), -vL[1] / mL * (s / mL))
vR = (vR[0] / mR * (s / mR), vR[1] / mR * (s / mR))
print(f"T {vT} {mT} L {vL} {mL} R {vR} {mR}")

rT = math.atan((0 - center2[1]) / (124.99999999 - center2[0])) - math.atan(vT[1] / vT[0])
rL = math.atan((triHeight - center2[1]) / (0 - center2[0])) - math.atan(vL[1] / vL[0])
rR = math.atan((triHeight - center2[1]) / (250 - center2[0])) - math.atan(vR[1] / vR[0])

vT = (math.cos(rT) * vT[0] - math.sin(rT) * vT[1], math.sin(rT) * vT[0] + math.cos(rT) * vT[1])
vL = (math.cos(rL) * vL[0] - math.sin(rL) * vL[1], math.sin(rL) * vL[0] + math.cos(rL) * vL[1])
vR = (math.cos(rR) * vR[0] - math.sin(rR) * vR[1], math.sin(rR) * vR[0] + math.cos(rR) * vR[1])
print(f"T {vT} {mT} L {vL} {mL} R {vR} {mR}")

randPointsX = []
randPointsY = []
for _ in range(0,5):
    randPointsX.append(random.randrange(640))
    randPointsY.append(random.randrange(480))

plt.figure()
plt.plot([calPoints[0][0], center[0], calPoints[1][0], center[0], calPoints[2][0]], [calPoints[0][1], center[1], calPoints[1][1], center[1], calPoints[2][1]], label='calPoints')
plt.scatter([center[0]], [center[1]], label='center')
plt.plot([vT[0] + center[0], center[0]], [vT[1] + center[1], center[1]], label="vT")
plt.plot([vL[0] + center[0], center[0]], [vL[1] + center[1], center[1]], label="vL")
plt.plot([vR[0] + center[0], center[0]], [vR[1] + center[1], center[1]], label="vR")
plt.scatter(randPointsX, randPointsY, label = "random inputs")
plt.axis('square')
plt.gca().set_aspect('equal', adjustable='box')
plt.gca().invert_yaxis()
plt.legend()

resultT = []
for i in range(len(randPointsX)):
    resultT.append(randPointsX[i] * vT[0] + randPointsY[i] * vT[1])

resultL = []
for i in range(len(randPointsX)):
    resultL.append((randPointsX[i] * vL[0] + randPointsY[i] * vL[1]))

resultR = []
for i in range(len(randPointsX)):
    resultR.append(randPointsX[i] * vR[0] + randPointsY[i] * vR[1])

plt.figure()
plt.plot([125, center2[0], 0, center2[0], 250], [0, center2[1], triHeight, center2[1], triHeight], label="servos")
plt.plot([vT[0] + center2[0], center2[0]], [vT[1] + center2[1], center2[1]], label="vT")
plt.plot([vL[0] + center2[0], center2[0]], [vL[1] + center2[1], center2[1]], label="vL")
plt.plot([vR[0] + center2[0], center2[0]], [vR[1] + center2[1], center2[1]], label="vR")
plt.scatter([125] * len(randPointsX), resultT, label = "result T")
plt.scatter([0] * len(randPointsX), resultL, label = "result L")
plt.scatter([250] * len(randPointsX), resultR, label = "result R")
plt.axis('square')
plt.gca().set_aspect('equal', adjustable='box')
plt.gca().invert_yaxis()
plt.legend()

# plt.figure()
# plt.axis('square')
# plt.plot(x, y, label='ball pos')
# plt.gca().set_aspect('equal', adjustable='box')
# plt.legend()

plt.show()