#!/usr/bin/env python

import numpy as np
from matplotlib import rcParams
rcParams['mathtext.fontset'] = 'stixsans'
from matplotlib import pyplot
import vfpic

pyplot.ion ()

cfg = vfpic.Config ()

B2 = []
t = []
for state in vfpic.DataFile (filename = "var.dat"):
  B2.append ((state.Bx.trim()**2 + state.By.trim()**2).mean ())
  t.append (state.it*cfg.dt)

B2 = np.array (B2)
t = np.array (t)

gamma = 0.75

pyplot.clf ()
(p1,) = pyplot.semilogy (t/(2*np.pi), np.sqrt (B2), 'k')
(p2,) = pyplot.semilogy (t/(2*np.pi), 7e-9*np.exp (gamma*t), 'r--')
pyplot.xlim (t[0]/(2*np.pi), t[-1]/(2*np.pi))
pyplot.ylim (5e-6, 10)
pyplot.xlabel ("Orbits")
pyplot.legend ((p1, p2), ("Numerical", "$\propto\,\exp (3\Omega t/4)$"),
    loc = "upper left")
pyplot.tight_layout ()

pyplot.draw ()

