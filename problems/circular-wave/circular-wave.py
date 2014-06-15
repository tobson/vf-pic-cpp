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
kz = 2*np.pi*cfg.ikz/cfg.Lz

k2 = kx**2 + kz**2

kvA2 = k2*vA2

Omegac = cfg.em*cfg.B0

omega = (0.5*kvA2/Omegac)*(np.sqrt(1.0 + 4.0*Omegac*Omegac/kvA2) + 1.0);
fac = np.sqrt (k2)*cfg.ampl/omega

phi = kx*x + kz*z + 0.5*omega*cfg.dt

plt.clf()
plots = plt.plot (x, np.zeros (cfg.nx), 'k',
                  x, np.zeros (cfg.nx), 'r')
plt.xlim (cfg.x0, cfg.x0 + cfg.Lx)
#plots = plt.plot (z, np.zeros (cfg.nz), 'k',
#                  z, np.zeros (cfg.nz), 'r')
#plt.xlim (cfg.z0, cfg.z0 + cfg.Lz)
plt.ylim (-1.1*fac, 1.1*fac)

for state in vfpic.DataFile (filename = "var.dat"):
  #t = state.it*cfg.dt
  phi = kx*state.x + kz*state.z - (state.it - 0.5)*omega*cfg.dt
  Ay = (cfg.ampl/omega)*np.cos (phi)
  By = np.sqrt (k2)*Ay
  plots[0].set_ydata (state.By.trim ().squeeze ())
  plots[1].set_ydata (By.trim ().squeeze ())
  plt.draw()
  print state.it
  #break

