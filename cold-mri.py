#!/usr/bin/env python

from numpy import *
from numpy.linalg import eig
import vfpic

# Read config and pull into global namespace
cfg = vfpic.Config ()
globals ().update (cfg)

# Real and complex data types
#rtype = dtype ('f' + str (  cfg.precision))
#ctype = dtype ('c' + str (2*cfg.precision))

# Alfven speed
vA = sqrt (B0*B0/rho0)

# Inverse cyclotron frequency
tau = 1./(em*B0)

# Highest unstable wave number
kvA_max = Omega*sqrt (2.*qshear/(1. - 2.*Omega*tau))

# Most unstable wave number
kvA_opt = Omega*sqrt (((4. - qshear)*qshear)/(2.*(4. - qshear)*Omega*tau + 4.))

# Most unstable wave number on the grid
kvA_grid = (2.*pi*vA/Lz)*round (kvA_opt*Lz/(2.*pi*vA))

def matrix (kvA):
  return array ([[0.                 , 2.*Omega, 1j*kvA                    , 0.          ],
                 [(qshear - 2.)*Omega, 0.      , 0.                        , 1j*kvA      ],
                 [1j*kvA             , 0.      , 0.                        , -kvA*kvA*tau],
                 [0.                 , 1j*kvA  , kvA*kvA*tau - qshear*Omega, 0.          ]])

vals, vecs = linalg.eig (matrix (kvA_grid))
n = argmax (vals.real)
kz = kvA_grid/vA

# Complex growth rate
gamma = vals[n]

# Eigenvector
ux, uy, bx, by = vecs[:,n]

# Always start from the same seed
random.seed (0)

##################################
# Initialize particles positions #
##################################

particles = vfpic.Particles (cfg)

if randomizePositions:
    particles["x"] = random.uniform (x0, x0 + Lx, npar)
    particles["z"] = random.uniform (z0, z0 + Lz, npar)
else:
    exponent = int (log2 (npc))
    assert (2**exponent == npc)
    npx = nx*(2**(           exponent/2))
    npz = nz*(2**(exponent - exponent/2))
    particles["x"], particles["z"] = \
        map (ravel, meshgrid (x0 + (arange(npx) + .5)*Lx/npx,
                              z0 + (arange(npz) + .5)*Lz/npz))

###################################
# Initialize particles velocities #
###################################

# Maxwellian
particles["vx"][...] = cs0*random.randn (npar)
particles["vy"][...] = cs0*random.randn (npar)
particles["vz"][...] = cs0*random.randn (npar)

# Add wave
phase = 1j*kz*particles["z"] - .5*gamma*dt

particles["vx"][...] += ampl*(ux*exp (phase)).real
particles["vy"][...] += ampl*(uy*exp (phase)).real

# Add shear
particles["vy"][...] -= qshear*Omega*particles["x"]

###############################
# Initialize vector potential #
###############################

Ax = vfpic.ScalarField (cfg)
Ay = vfpic.ScalarField (cfg)
Az = vfpic.ScalarField (cfg)

phase = 1j*kz
