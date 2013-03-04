#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot
from copy import deepcopy
import vfpic

pyplot.ion ()

cfg = vfpic.Config ()

try:
  states[0].it
except:
  states = []
  for state in vfpic.DataFile (filename = "var.dat"):
    if state.it % 1024 == 0: states.append (deepcopy (state))

pyplot.clf ()

#nrows, ncols = 2, 5
#fig, axes = pyplot.subplots (nrows = nrows, ncols = ncols)
#for j in range (nrows):
#  for i in range (ncols):
#    axes[j,i].imshow (states[j*ncols + i].Bx.trim ())
#pyplot.tight_layout ()

nrows, ncols = 1, 5
fig, axes = pyplot.subplots (num = 1, nrows = nrows, ncols = ncols)
for i in range (ncols):
  state = states[2 + i]
  # Plot Ax!
  # Plot x component of transverse A only
  axes[i].imshow (state.Bx.trim ())
  axes[i].set_xticklabels ("")
  axes[i].set_yticklabels ("")
  axes[i].set_title ("%.2f" % (state.it*cfg.dt/(2.*np.pi)))
pyplot.tight_layout ()
