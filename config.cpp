//
//  config.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"

#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <yaml-cpp/yaml.h>

#include "global.h"

namespace config
{
    /* Grid */
    real x0 = -0.5;
    real z0 = -0.5;

    real Lx = 1.0;
    real Lz = 1.0;

    /* Time stepping */
    real dt = 1e-3;
    long long nt = 1000;

    /* Physics */
    real rho0 = 1.0;
    real cs0 = 1.0;
    real em = 1.0;

    /* Shearing sheet */
    real Omega = 0.0;
    real qshear = 0.0;

    /* Initial condition */
    int ikx = 0;
    int ikz = 0;
    real B0 = 0.0;
    real brms = 0.0;
    real ampl = 0.0;
    bool randomizeSeed = false;
    bool randomizePositions = true;
    std::string initcond;

    /* Diagnostics and data I/O */
    int itWrite = 1;
    int itPrint = 1;

    bool writeSources = false;
    bool writeData = true;
    bool writeDiagnostics = true;
    bool writeParticles = true;

    bool overwriteSnapshots = false;

    /* Smoothing */
    bool filterDensity = false;
    bool filterVelocity = false;
    bool filterEfield = false;
    bool filterBfield = false;

    /* Debugging */
    bool verbose = false;
    bool throwOnCopyConstruct = false;

    struct ReadVariable
    {
        ReadVariable (const std::string& srcdir):
        config (YAML::LoadFile (srcdir + "/input.yaml")),
        output (srcdir + "/output.yaml")
        {
            /* Store precision */
            config["precision"] = sizeof (real);

            /* Append parameters */
            config["nx"] = vfpic::nx;
            config["nz"] = vfpic::nz;
            config["npc"] = vfpic::npc;
            config["nthreads"] = vfpic::nthreads;
            config["alignment"] = vfpic::alignment;

            /* Also append global constants */
            config["mx"] =  vfpic::mx;
            config["mz"] = vfpic::mz;
            config["npar"] = vfpic::npar;
            config["mpar"] = vfpic::mpar;
        }
        template <typename T>
        void operator() (const std::string& name, T& value)
        {
            value = config[name].as<T> (value);
            config[name] = value;
        }
        ~ReadVariable ()
        {
            output << YAML::Dump (config) << std::endl;
            output.close ();
        }
        YAML::Node config;
        std::ofstream output;
    };
    
    template <typename T>
    void readVariable (YAML::Node& node, const std::string& name, T& value)
    {
        value = node[name].as<T> (value);
        node[name] = value;
    }

    void read (const std::string& srcdir)
    {
        ReadVariable readVariable (srcdir);

        /* Grid */
        readVariable ("x0", x0);
        readVariable ("z0", z0);

        readVariable ("Lx", Lx);
        readVariable ("Lz", Lz);

        /* Time stepping */
        readVariable ("dt", dt);
        readVariable ("nt", nt);

        /* Physics */
        readVariable ("rho0", rho0);
        readVariable ("cs0", cs0);
        readVariable ("em", em);

        /* Shearing sheet */
        readVariable ("Omega", Omega);
        readVariable ("qshear", qshear);

        /* Initial condition */
        readVariable ("ikx", ikx);
        readVariable ("ikz", ikz);
        readVariable ("B0", B0);
        readVariable ("brms", brms);
        readVariable ("ampl", ampl);
        readVariable ("randomizeSeed", randomizeSeed);
        readVariable ("randomizePositions", randomizePositions);
        readVariable ("initcond", initcond);

        /* Diagnostics and data I/O */
        readVariable ("itWrite", itWrite);
        readVariable ("itPrint", itPrint);

        readVariable ("writeSources", writeSources);
        readVariable ("writeData", writeData);
        readVariable ("writeDiagnostics", writeDiagnostics);
        readVariable ("writeParticles", writeParticles);

        readVariable ("overwriteSnapshots", overwriteSnapshots);

        /* Smoothing */
        readVariable ("filterDensity", filterDensity);
        readVariable ("filterVelocity", filterVelocity);
        readVariable ("filterEfield", filterEfield);
        readVariable ("filterBfield", filterBfield);

        /* Debugging */
        readVariable ("verbose", verbose);
        readVariable ("throwOnCopyConstruct", throwOnCopyConstruct);
    }
}
