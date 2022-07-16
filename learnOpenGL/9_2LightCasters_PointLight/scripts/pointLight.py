import numpy as np
import matplotlib.pyplot as plt


class Light:
    class Attenuation:
        def __init__(self, k_c, k_l, k_q) -> None:
            self.K_C = k_c
            self.K_L = k_l
            self.K_Q = k_q

    def make_attenuation(self, k_c, k_l, k_q):
        return self.Attenuation(k_c, k_l, k_q)


light = Light()
# data from http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
attenuation65 = light.make_attenuation(1.0, 0.07, 0.017)
attenuation100 = light.make_attenuation(1.0, 0.045, 0.0075)
attenuation160 = light.make_attenuation(1.0, 0.027, 0.0028)
attenuation200 = light.make_attenuation(1.0, 0.022, 0.0019)
attenuation600 = light.make_attenuation(1.0, 0.007, 0.0002)


def pointLight_attenuation(distance, k_c, k_l, k_q):
    return 1.0 / (k_c + k_l * distance + k_q * np.power(distance, 2.0))


CONSTANT_V = 1.0
K_C = 1.0
K_L = 0.7
K_Q = 1.8


distances = np.arange(0, 100, 1.0)
attenuationV = pointLight_attenuation(
    distances, attenuation100.K_C, attenuation100.K_L, attenuation100.K_Q)
plt.scatter(distances, attenuationV)
plt.xlabel("distance")
plt.ylabel("attenuation")
plt.title("Light Attenuation")
plt.grid()
plt.show()
