#!/usr/bin/env python

import numpy as np
#from matplotlib import use
#use ("Agg")
from matplotlib import pyplot as plt
import vfpic

cfg = vfpic.Config ()

plt.ion ()

var = "rux"

plt.clf ()

it = 0

for state in vfpic.DataFile ():
  print state.it
  if state.it == 0:
    p = plt.imshow (state[var].trim (), extent=[1,cfg.nx,1,cfg.nz])
    nticks = 4
    plt.xticks ((cfg.nx/nticks)*np.arange (1,nticks + 4))
    plt.xlim (1,cfg.nx)
    plt.yticks ((cfg.nz/nticks)*np.arange (1,nticks + 4))
    plt.ylim (1,cfg.nz)
    plt.tight_layout ()
  else:
    p.set_data (state[var].trim ())
    plt.draw ()
  plt.savefig ("video/%04d.png" % it)
  it += 1
