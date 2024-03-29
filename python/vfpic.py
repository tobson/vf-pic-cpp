#!/usr/bin/env python

from numpy import ndarray
from collections import OrderedDict

class Config (dict):

    def __init__ (self, filename = "output.cfg"):

        from numpy import dtype
        from ConfigParser import SafeConfigParser
        from ast import literal_eval

        parser = SafeConfigParser ()
        parser.optionxform = str

        parser.read ("output.cfg")

        for section in parser.sections ():

          for item in parser.items (section):

            key, string = item

            try:
              self[key] = literal_eval (string)
            except:
              try:
                self[key] = parser.getboolean (section, key)
              except:
                self[key] = string

        self["dx"] = self["Lx"]/self["nx"]
        self["dz"] = self["Lz"]/self["nz"]

        self["npar"] = self["nx"]*self["nz"]*self["npc"]

        self["rtype"] = dtype ("f" + str (  self["precision"]))
        self["ctype"] = dtype ("c" + str (2*self["precision"]))

        # Allow e.g. cfg["dx"] to be accessed through cfg.dx
        self.__dict__.update (self)

class Particles (ndarray):

    def __new__ (cls, cfg = None):
        if not isinstance (cfg, Config): cfg = Config ()
        dtype = [( 'x', cfg.rtype),
                 ( 'z', cfg.rtype),
                 ('vx', cfg.rtype),
                 ('vy', cfg.rtype),
                 ('vz', cfg.rtype)]
        obj = ndarray.__new__ (cls, shape = (cfg.npar,), dtype = dtype)
        return obj
  
    def __array_finalize__ (self, obj):
        if obj is None: return
  
    def read (self, f):
        from numpy import fromfile
        self[...] = fromfile (f, self.dtype, self.size)
        return self.size*self.itemsize

class ScalarField (ndarray):

    def __new__ (cls, cfg = None):
        if not isinstance (cfg, Config): cfg = Config ()
        cls.nghost = cfg.nghost
        shape = (cfg.nz + 2*cfg.nghost, cfg.nx + 2*cfg.nghost)
        obj = ndarray.__new__ (cls, shape = shape, dtype = cfg.rtype)
        return obj

    def __array_finalize__ (self, obj):
        if obj is None: return

    def read (self, f):
        from numpy import fromfile
        self[...] = fromfile (f, self.dtype, self.size).reshape (self.shape)
        return self.size*self.itemsize

    def trim (self):
        return self[self.nghost:-self.nghost,self.nghost:-self.nghost]

class Grid ():

    def __init__ (self, cfg = None):

        from numpy import meshgrid, arange

        if not isinstance (cfg, Config): cfg = Config ()

        self.x = ScalarField (cfg)
        self.z = ScalarField (cfg)

        self.x[...], self.z[...] = \
            meshgrid (cfg.x0 + (arange (cfg.nx + 2*nghost) - 0.5)*cfg.Lx/cfg.nx,
                      cfg.z0 + (arange (cfg.nz + 2*nghost) - 0.5)*cfg.Lz/cfg.nz)

class State (OrderedDict):

    def __init__ (self, cfg = None):

        from numpy import ndarray

        super (State, self).__init__ ()

        if not isinstance (cfg, Config): cfg = Config ()

        self["Ax"] = ScalarField (cfg)
        self["Ay"] = ScalarField (cfg)
        self["Az"] = ScalarField (cfg)

        self["Bx"] = ScalarField (cfg)
        self["By"] = ScalarField (cfg)
        self["Bz"] = ScalarField (cfg)

        self["Ex"] = ScalarField (cfg)
        self["Ey"] = ScalarField (cfg)
        self["Ez"] = ScalarField (cfg)

        if cfg.writeParticles: self["particles"] = Particles (cfg)

        self["rho"] = ScalarField (cfg)

        self["rux"] = ScalarField (cfg)
        self["ruy"] = ScalarField (cfg)
        self["ruz"] = ScalarField (cfg)
        
        self["x"] = ScalarField (cfg)
        self["z"] = ScalarField (cfg)

        self.__dict__.update (self)
        
        self.it = 0

    def read (self, f):

        from numpy import fromfile, int64

        recordsize = 0

        # Read the various fields
        for key, value in self.items (): recordsize += value.read (f)

        # Read time
        self.it, = fromfile (f, dtype = int64, count = 1)
        recordsize += self.it.nbytes
        
        return recordsize

class DataFile ():

    def __init__ (self, filename = "var.dat", cfg = None):

        if not isinstance (cfg, Config): cfg = Config ()

        self.f = open (filename)

        # Determine end of file
        self.f.seek (0, 2)
        self.eof = self.f.tell ()
        self.f.seek (0)

        self.state = State (cfg)

        self.recordsize = 0

    def __iter__ (self):

        while self.eof - self.f.tell () >= self.recordsize:

            self.recordsize = self.state.read (self.f)

            yield self.state

    def __del__ (self):

        print "pos = ", self.f.tell()
        print "eof = ", self.eof
        print "recordsize = ", self.recordsize
        self.f.close ()

class Diagnostics (dict):

    def __init__ (self, names, data, cfg = None):

        from numpy import arange

        if not isinstance (cfg, Config): cfg = Config ()

        nvar = len (names)
        assert (data.shape[1] == nvar)
        self["t"] = cfg.dt*cfg.itDiag*arange (data.shape[0])
        for ivar in range (nvar):
            self[names[ivar]] = data[:,ivar].copy ()
        self.__dict__.update (self)

def readDiagnostics (filename = "diag.txt", cfg = None):

    from numpy import loadtxt

    if not isinstance (cfg, Config): cfg = Config ()

    f = open (filename, "rt")
    names = f.readline ().lstrip ("# ").rstrip (" \n").split ("\t")
    data = loadtxt (f)
    f.close ()

    return Diagnostics (names, data)
