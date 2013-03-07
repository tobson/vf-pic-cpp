#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot as plt
import vfpic

cfg = vfpic.Config ()

kx = 2*np.pi*cfg.ikx/cfg.Lx
kz = 2*np.pi*cfg.ikz/cfg.Lz

angle = np.arctan2 (kx, kz);

k2 = kx**2 + kz**2
kvA = np.sqrt (k2/cfg.rho0)*cfg.B0

name = "By"

plt.ion ()

ampl = []
rms = []

for state in vfpic.DataFile ():

  var = state[name].trim ()

  x = state.x.trim ()
  z = state.z.trim ()

  Bx = state.Bx.trim () - cfg.B0*np.sin (angle)
  By = state.By.trim ()
  Bz = state.Bz.trim () - cfg.B0*np.cos (angle)

  #fft = np.fft.rfft2 (var)
  #ampl.append (np.abs (fft[1,1]))

  fft = np.fft.rfft (var.flatten ())
  ampl.append (np.abs (fft[1]))

  rms.append (np.sqrt ((Bx**2 + By**2 + Bz**2).mean ()))
  #break

gamma = -0.15017554368990996j

ampl = np.array (ampl)
rms = np.array (rms)
t = np.arange (ampl.size)*cfg.dt
theory = ampl[0]*np.exp (gamma*t)
