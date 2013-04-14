#!/usr/bin/env python

import numpy as np
from numpy.fft import fftfreq, rfft2, irfft2
from matplotlib import pyplot as plt

def rfftfreq (n):
  from numpy import arange
  return arange (n/2 + 1, dtype=float)/n

def xder (f):
  return (np.roll (f, -1, 1) - np.roll (f, +1, 1))/(2*dx)
def zder (f):
  return (np.roll (f, -1, 0) - np.roll (f, +1, 0))/(2*dz)

def xder2 (f):
  return (np.roll (f, +1, 1) - 2*f + np.roll (f, -1, 1))/(dx**2)
def zder2 (f):
  return (np.roll (f, +1, 0) - 2*f + np.roll (f, -1, 0))/(dz**2)

def Ohm (A, E):
  E[0] = -nu*(xder (xder (A[0])) + zder (xder (A[1])))
  E[1] = -nu*(xder (zder (A[0])) + zder (zder (A[1])))

def iterate (A):

  from numpy import empty_like, sqrt

  E = empty_like (A)
  E2 = empty_like (A)
  D = empty_like (A)
  D2 = empty_like (A)

  Ohm (A, E)

  A += 0.5*dt*E

  t, divA = [], []

  for it in range (nt):

    # Divergence
    t.append (it*dt)
    divA.append (sqrt (((xder (A[0]) + zder (A[1]))**2).mean ()))

    # Advance vector potential to n+1/2
    A -= dt*E

    # Compute electric field at n+1/2
    Ohm (A, D)

    # Predict electric field at n+1
    E2 = 2*D - E

    # Advance copy of vector potential to n+3/2
    A2 = A - dt*E2

    # Compute electric field at n+3/2
    Ohm (A2, D2)

    # Correct electric field at n+1
    E = 0.5*(D + D2)

  return np.array ((t, divA))

def divclean (A):

  k2 = kx1**2 + kz1**2
  k2[0,0] = 1.0
  k21 = 1.0/k2
  k21[0,0] = 0.0
  k2[0,0] = 0.0

  Ax = rfft2 (A[0])
  Az = rfft2 (A[1])

  divA = k21*(kx1*Ax + kz1*Az)
  Ax -= kx1*divA
  Az -= kz1*divA

  return irfft2 (np.array ((Ax, Az)))

nx, nz = 128, 128
Lx, Lz = 1.0, 1.0
dt = 1e-3
nt = 1024

dx, dz = Lx/nx, Lz/nz
nu = 0.25*(dx**2)/dt

kx1, kz1 = np.meshgrid (np.sin (2*np.pi*rfftfreq (nx))/dx,
                        np.sin (2*np.pi* fftfreq (nz))/dz)

A = np.random.randn (2, nz, nx)
A = divclean (A)

if True:

  By = zder (A[0]) - xder (A[1])
  Jx = zder2 (A[0]) - xder (zder (A[1]))
  Jz = xder2 (A[1]) - zder (xder (A[0]))

  A1 = A.copy ()
  t, divA = iterate (A1)
  By1 = zder (A1[0]) - xder (A1[1])
  Jx1 = zder2 (A1[0]) - xder (zder (A1[1]))
  Jz1 = xder2 (A1[1]) - zder (xder (A1[0]))

  plt.ion ()
  plt.clf ()
  plt.semilogy (t, divA, 'k')
  plt.draw ()
