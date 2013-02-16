#!/usr/bin/env python

from numpy import ndarray

class Config:

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

class Particles (ndarray):

    def __new__ (cls, config = None):
        from numpy import float32
        if not isinstance (config, Config): config = Config ()
        dtype = [( 'x',float32),( 'z',float32),
                 ('vx',float32),('vy',float32),('vz',float32)]
        obj = ndarray.__new__ (cls, shape = (config.npar,), dtype = dtype)
        return obj
  
    def __array_finalize__ (self, obj):
        if obj is None: return
  
    def read (self, f):
        from numpy import fromfile
        self[...] = fromfile (f, self.dtype, self.size)
        return self.size*self.itemsize

class ScalarField (ndarray):

    def __new__ (cls, config = None):
        from numpy import float32
        if not isinstance (config, Config): config = Config ()
        dtype = float32
        shape = (config.nz + 2, config.nx + 2)
        obj = ndarray.__new__ (cls, shape = shape, dtype = dtype)
        return obj

    def __array_finalize__ (self, obj):
        if obj is None: return

    def read (self, f):
        from numpy import fromfile
        self[...] = fromfile (f, self.dtype, self.size).reshape (self.shape)
        return self.size*self.itemsize

    def trim (self):
        return self[1:-1,1:-1]

class State:

    def __init__ (self, config = None):

        from numpy import ndarray, float32

        if not isinstance (config, Config): config = Config ()

        self.Ax = ScalarField (config)
        self.Ay = ScalarField (config)
        self.Az = ScalarField (config)

        self.Bx = ScalarField (config)
        self.By = ScalarField (config)
        self.Bz = ScalarField (config)

        self.Ex = ScalarField (config)
        self.Ey = ScalarField (config)
        self.Ez = ScalarField (config)

        self.particles = Particles (config)

        self.rho = ScalarField (config)

        self.rux = ScalarField (config)
        self.ruy = ScalarField (config)
        self.ruz = ScalarField (config)
        
        self.x = ScalarField (config)
        self.z = ScalarField (config)
        
        self.it = 0
        #self.t = 0.0

    def read (self, f):

        from numpy import fromfile, int64, float32

        recordsize = 0

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

        # Read particles
        recordsize += self.particles.read (f)

        # Read sources
        recordsize += self.rho.read (f)

        recordsize += self.rux.read (f)
        recordsize += self.ruy.read (f)
        recordsize += self.ruz.read (f)
        
        # Read grid
        recordsize += self.x.read (f)
        recordsize += self.z.read (f)

        # Read time
        self.it, = fromfile (f, dtype = int64, count = 1)
        recordsize += self.it.nbytes
        
        return recordsize

class DataFile ():

    def __init__ (self, config = None, filename = "data/var.dat"):

        if not isinstance (config, Config): config = Config ()

        self.f = open (filename)

        # Determine end of file
        self.f.seek (0, 2)
        self.eof = self.f.tell ()
        self.f.seek (0)

        self.state = State (config)

        self.recordsize = 0
        print "eof = ", self.eof

    def __iter__ (self):

        while self.eof - self.f.tell () >= self.recordsize:

            self.recordsize = self.state.read (self.f)
            print "recordsize = ", self.recordsize

            yield self.state

    def __del__ (self):

        self.f.close ()
