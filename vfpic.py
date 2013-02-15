#!/usr/bin/env python

from numpy import ndarray

class config:

    def __init__ (self, filename = "config.out"):

        from numpy import sqrt

        f = open (filename, "rt")

        for line in f.readlines ():
            
            varname, value = line.split("=")
            value = value.strip().lower()
            if value.find("false") >= 0: value = "False"
            if value.find("true" ) >= 0: value = "True"
            exec ("self." + varname.strip() + "=" + value)

        self.dx = self.Lx/self.nx
        self.dz = self.Lz/self.nz

        self.npar = self.nx*self.nz*self.npc

        f.close ()

class State:

    def __init__ (self, config = None):

        from numpy import ndarray, float64

        if not isinstance (config, Config): config = Config ()

        self.lwrite_sources = config.lwrite_sources

        self.particles = Particles (config = config)

        if self.lwrite_sources:

            self.rho = ScalarField ()

            self.rux = ScalarField ()
            self.ruy = ScalarField ()
            self.ruz = ScalarField ()

        self.Ax = ScalarField ()
        self.Ay = ScalarField ()
        self.Az = ScalarField ()

        self.Bx = ScalarField ()
        self.By = ScalarField ()
        self.Bz = ScalarField ()

        self.Ex = ScalarField ()
        self.Ey = ScalarField ()
        self.Ez = ScalarField ()
        
        self.x = ndarray ((config.mx,), dtype = float64)
        self.z = ndarray ((config.mz,), dtype = float64)
        
        self.it = 0
        self.t = 0.0

    def read (self, f):

        from numpy import fromfile, int64, float64

        recordsize = 0

        # Read particles
        recordsize += self.particles.read (f)

        # Read sources
        if self.lwrite_sources:

            recordsize += self.rho.read (f)

            recordsize += self.rux.read (f)
            recordsize += self.ruy.read (f)
            recordsize += self.ruz.read (f)

        # Read fields
        recordsize += self.Ax.read (f)
        recordsize += self.Ay.read (f)
        recordsize += self.Az.read (f)

        recordsize += self.Bx.read (f)
        recordsize += self.By.read (f)
        recordsize += self.Bz.read (f)

        recordsize += self.Ex.read (f)
        recordsize += self.Ey.read (f)
        recordsize += self.Ez.read (f)
        
        # Read grid
        self.x[...] = fromfile (f, dtype = float64, count = self.x.size)
        self.z[...] = fromfile (f, dtype = float64, count = self.z.size)
        
        # Read time step
        self.it, = fromfile (f, dtype = int64, count = 1)
        recordsize += self.it.nbytes
        
        # Read time
        self.t, = fromfile (f, dtype = float64, count = 1)
        recordsize += self.t.nbytes
        
        return recordsize
