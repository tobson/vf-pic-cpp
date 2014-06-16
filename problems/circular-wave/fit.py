import vfpic
import numpy as np
from scipy import optimize
from matplotlib import pyplot as plt

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

plt.ion ()

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

By_p = np.array ([1.,0.])
Bz_p = np.array ([1.,0.])
uy_p = np.array ([1.,0.])
uz_p = np.array ([1.,0.])

ampl = []
phase = []

for state in vfpic.DataFile (filename = "var.dat"):

  x = state.x.trim ().squeeze ()
  phi = lambda p1: kx*x - (state.it - 0.5)*omega*cfg.dt + p1

  By = lambda p:  p[0]*kx*A_hat*np.cos (phi (p[1]))
  Bz = lambda p: -p[0]*kx*A_hat*np.sin (phi (p[1]))
  uy = lambda p:  p[0]*u_hat*np.cos (phi (p[1]))
  uz = lambda p: -p[0]*u_hat*np.sin (phi (p[1]))

  rho = state.rho.trim ().squeeze ()
  state_uy = state.ruy.trim ().squeeze ()/rho
  state_uz = state.ruz.trim ().squeeze ()/rho
  state_By = state.By.trim ().squeeze ()
  state_Bz = state.Bz.trim ().squeeze ()

  By_err = lambda p: state_By - By (p)
  Bz_err = lambda p: state_Bz - Bz (p)
  uy_err = lambda p: state_uy - uy (p)
  uz_err = lambda p: state_uz - uz (p)

  By_p, err = optimize.leastsq (By_err, By_p)
  Bz_p, err = optimize.leastsq (Bz_err, Bz_p)
  uy_p, err = optimize.leastsq (uy_err, uy_p)
  uz_p, err = optimize.leastsq (uz_err, uz_p)

  lines[0,0][0].set_ydata (state_By)
  lines[0,0][1].set_ydata (      By (By_p))
  lines[0,1][0].set_ydata (state_Bz)
  lines[0,1][1].set_ydata (      Bz (Bz_p))
  lines[1,0][0].set_ydata (state_uy)
  lines[1,0][1].set_ydata (      uy (uy_p))
  lines[1,1][0].set_ydata (state_uz)
  lines[1,1][1].set_ydata (      uz (uz_p))

  ampl.append (By_p[0])
  phase.append (By_p[1])

  plt.draw()

  print state.it

ampl = np.array (ampl)
phase = np.array (phase)

plt.ioff ()
