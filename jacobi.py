#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot as plt
from vfpic import config

cfg = config ()

shape = (1, 8, cfg.nz+2, cfg.nx+2)

if (cfg.precision == 4):
    dtype = np.float32
elif (cfg.precision == 8):
    dtype = np.float64
else:
    print "Error: Unkown precision"
    print "precision = ", precision

data = np.fromfile ("jacobi.dat", dtype=dtype).reshape (shape)

rho1 = data[0,0,:,:].copy()
J1 = data[0,1:4,:,:].copy()

phi1 = data[0,4,:,:].copy()
A1 = data[0,5:8,:,:].copy()

#rho2 = data[1,0,:,:].copy()
#J2 = data[1,1:4,:,:].copy()

#phi2 = data[1,4,:,:].copy()
#A2 = data[1,5:8,:,:].copy()

plt.ion ()

plt.clf()
fig, axes = plt.subplots (3, 4, num=1)

axes[0,0].imshow (phi1,aspect="auto")
for j in range (3): axes[0,j+1].imshow (A1[j,:,:],aspect="auto")

#axes[1,0].imshow (phi2,aspect="auto")
#for j in range (3): axes[1,j+1].imshow (A2[j,:,:],aspect="auto")

#axes[2,0].imshow (phi1 - phi2,aspect="auto")
#for j in range (3): axes[2,j+1].imshow (A1[j,:,:] - A2[j,:,:],aspect="auto")

for axis in axes.flatten ():
    axis.set_xticklabels ("")
    axis.set_yticklabels ("")

plt.tight_layout ()