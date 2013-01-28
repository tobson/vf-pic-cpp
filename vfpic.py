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