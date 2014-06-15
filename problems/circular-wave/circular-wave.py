import vfpic
import numpy as np
from matplotlib import pyplot as plt

plt.rcParams['image.origin'] = 'lower'

plt.ion()

cfg = vfpic.Config()

x = cfg.x0 + (np.arange(cfg.nx) + 0.5)*cfg.dx
z = cfg.z0 + (np.arange(cfg.nz) + 0.5)*cfg.dz

vA2 = cfg.B0**2/cfg.rho0

kx = 2*np.pi*cfg.ikx/cfg.Lx

k2 = kx**2

kvA = np.sqrt (k2*vA2)

oc = cfg.em*cfg.B0

hel = 1.0
omega = kvA*np.sqrt (1.0 + 0.25*kvA*kvA/(oc*oc)) + 0.5*kvA*kvA/(hel*oc)

u_hat = -cfg.ampl*np.sqrt (vA2)*kvA/omega
A_hat = cfg.ampl*cfg.B0/kx

phi = kx*x + 0.5*omega*cfg.dt

plt.clf()
plots = plt.plot (x, np.zeros (cfg.nx), 'k',
                  x, np.zeros (cfg.nx), 'r')
plt.xlim (cfg.x0, cfg.x0 + cfg.Lx)

for state in vfpic.DataFile (filename = "var.dat"):
  phi = kx*state.x - (state.it - 0.5)*omega*cfg.dt
  Ay = A_hat*np.cos (phi)
  By = np.sqrt (k2)*Ay
  plots[0].set_ydata (state.By.trim ().squeeze ())
  plots[1].set_ydata (By.trim ().squeeze ())
  plt.draw()
  print state.it

