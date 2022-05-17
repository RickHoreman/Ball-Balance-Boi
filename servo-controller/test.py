import math
import matplotlib.pyplot as plt

x = range(180)
y = []
for i in x:
    y.append(math.cos(math.radians(i)))

plt.plot(x, y)
plt.show()