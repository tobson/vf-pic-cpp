#!/usr/bin/env python

import numpy as np
import vfpic

cfg = vfpic.Config ()

t = np.empty (0)
rms = np.empty (0)
for state in vfpic.DataFile (cfg = cfg, filename = "var.dat"):
  print state.it
  t = np.append (t, cfg.dt*state.it)
  rms = np.append (rms, np.sqrt ((state.Ax.trim()**2).mean ()))

gamma = np.polyfit (t, np.log (rms), 1)[0]

print gamma
