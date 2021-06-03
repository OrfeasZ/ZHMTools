from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import random


fig = pyplot.figure()
ax = Axes3D(fig)

points = []

x = []
y = []
z = []

for point in points:
	x.append(point[0])
	y.append(point[1])
	z.append(point[2])

ax.scatter(x, y, z, s=10)
pyplot.show()