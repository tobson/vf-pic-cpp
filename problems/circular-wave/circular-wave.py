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
fig, axes = plt.subplots (ncols = 2, nrows = 2, num = 1,
    sharex = True, sharey = "row")
lines = np.empty (axes.shape, dtype = object)
for i in range (2):
  for j in range (2):
    lines[i,j] = axes[i,j].plot (x, np.zeros (cfg.nx), 'k',
                                 x, np.zeros (cfg.nx), 'r')
plt.xlim (cfg.x0, cfg.x0 + cfg.Lx)
axes[0,0].set_title ('By')
axes[0,1].set_title ('Bz')
axes[1,0].set_title ('uy')
axes[1,1].set_title ('uz')
plt.tight_layout ()
for ax in axes.flatten (): ax.title.set_y (1.02)

for state in vfpic.DataFile (filename = "var.dat"):

  x = state.x.trim ().squeeze ()
  phi = kx*x - (state.it - 0.5)*omega*cfg.dt

  uy =  u_hat*np.cos (phi)
  uz = -u_hat*np.sin (phi)
  By =  kx*A_hat*np.cos (phi)
  Bz = -kx*A_hat*np.sin (phi)

  rho = state.rho.trim ().squeeze ()
  state_uy = state.ruy.trim ().squeeze ()/rho
  state_uz = state.ruz.trim ().squeeze ()/rho
  state_By = state.By.trim ().squeeze ()
  state_Bz = state.Bz.trim ().squeeze ()

  lines[0,0][0].set_ydata (state_By)
  lines[0,0][1].set_ydata (      By)
  lines[0,1][0].set_ydata (state_Bz)
  lines[0,1][1].set_ydata (      Bz)
  lines[1,0][0].set_ydata (state_uy)
  lines[1,0][1].set_ydata (      uy)
  lines[1,1][0].set_ydata (state_uz)
  lines[1,1][1].set_ydata (      uz)

  plt.draw()
  print state.it
