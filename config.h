//
//  config.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__config__
#define __vf_pic__config__

#include "param.h"

#include <array>
#include <string>

namespace config
{
/* Grid */
extern real x0;
extern real z0;

extern real Lx;
extern real Lz;

/* Time stepping */
extern real dt;
extern long long nt;

/* Physics */
extern real rho0;
extern real cs0;
extern real em;

/* Shearing sheet */
extern real Omega;
extern real qshear;

/* Initial condition */
extern int ikx;
extern int ikz;
extern std::array<real,3> B0;
extern real brms;
extern real ampl;
extern bool randomizeSeed;
extern std::string initcond;

/* Diagnostics and data I/O */
extern int itWrite;
extern int itPrint;

extern bool writeSources;
extern bool writeData;
extern bool writeDiagnostics;

extern bool overwriteSnapshots;

/* Smoothing */
extern bool filterDensity;
extern bool filterVelocity;
extern bool filterEfield;
extern bool filterBfield;

/* Debugging */
extern bool verbose;
extern bool throwOnCopyConstruct;

/* Read configuration from input file */
void read (const std::string&);

/* Convenience functions to read variable from configuration file */
void readVariable (const char *, int &);
void readVariable (const char *, long long &);
void readVariable (const char *, real &);
void readVariable (const char *, bool &);
void readVariable (const char *, std::string&);

/* Write configuration to output file */
void write (const std::string&);
}

#endif /* defined(__vf_pic__config__) */
