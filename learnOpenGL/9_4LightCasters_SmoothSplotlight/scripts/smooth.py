from turtle import color
import numpy as np
from matplotlib import pyplot as plt
import matplotlib


def func_I(_innerCutOff, _outCutOff, _theta):
    cons = np.pi / 180.0
    return (np.cos(_theta * cons) - np.cos(_outCutOff * cons)) / (np.cos(_innerCutOff * cons) - np.cos(_outCutOff * cons))


innerCutOff = 25
outCutOff = 35
theta = np.arange(20.0, 40.0, 0.5)

funcV = func_I(innerCutOff, outCutOff, theta)

plt.scatter(theta, funcV, color="g")
plt.xlabel(r"$\theta$")
plt.ylabel("I")
plt.title(r"smooth")
plt.grid()
plt.text(25, 0.2, "inner={ico}, outter={oco}".format(
    ico=innerCutOff, oco=outCutOff), color="g")

plt.axvline(x=25.0, color="r", ls='--', lw=2)
plt.axvline(x=35.0, color='r', ls='--', lw=2)
plt.axhline(y=0.0, color='r', ls='--', lw=2)
plt.axhline(y=1.0, color='r', ls='--', lw=2)
plt.show()
