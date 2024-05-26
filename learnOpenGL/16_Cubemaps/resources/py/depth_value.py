import numpy as np
import matplotlib.pyplot as plt


z_value = np.arange(1.0, 50.0, 0.1, dtype=float)
far = 50
near = 1.0

func_depth = ((1 / z_value) - (1 / near)) / ((1 / far - 1/near))
plt.plot(z_value, func_depth)
plt.title("near=1,far=50")
plt.xlabel("Z-value")
plt.ylabel("Depth value")
plt.grid()
plt.show()
