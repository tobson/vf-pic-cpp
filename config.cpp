//
//  config.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"

#include "global.h"

#include <libconfig.h++>
#include <iostream>


namespace config
{
    libconfig::Config input, output;

    using libconfig::Setting;

    template <typename T> struct TypeMap;
    template <> struct TypeMap<int>       { static const Setting::Type type = Setting::TypeInt;     };
    template <> struct TypeMap<long long> { static const Setting::Type type = Setting::TypeInt64;   };
    template <> struct TypeMap<real>      { static const Setting::Type type = Setting::TypeFloat;   };
    template <> struct TypeMap<bool>      { static const Setting::Type type = Setting::TypeBoolean; };

    /* Convenience functions to read variable from configuration file */
    template <typename T>
    void readVariable (const std::string& name, T& value)
    {
        if (input.exists (name))
        {
            try
            {
                value = input.lookup (name);
            }
            catch (const libconfig::SettingTypeException)
            {
                std::cout << "Error reading config file: Variable '" << name
                << "' doesn't have the right type." << std::endl;
                throw;
            }
        }
        output.getRoot ().add (name, TypeMap<T>::type) = value;
    }

    void readVariable (const std::string& name, std::string& value)
    {
        if (input.exists (name))
        {
            try
            {
                value = input.lookup (name).c_str ();
            }
            catch (const libconfig::SettingTypeException)
            {
                std::cout << "Error reading config file: Variable '" << name
                << "' doesn't have the right type." << std::endl;
                throw;
            }
        }
        output.getRoot ().add (name, Setting::TypeString) = value.c_str ();
    }

    template <typename T, unsigned long N>
    void readVariable (const std::string& name, std::array<T,N>& array)
    {
        if (input.exists (name))
        {
            try
            {
                Setting& setting = input.lookup (name);
                if (setting.getLength () == N)
                {
                    for (int n = 0; n < N; ++n) array[n] = setting[n];
                }
                else
                {
                    std::string msg = "'" + std::string (name) + "' should be an array of length " + std::to_string (N) + ".";
                    throw std::length_error (msg);
                }
            }
            catch (const libconfig::SettingTypeException)
            {
                std::cout << "Error reading config file: Variable '" << name
                << "' doesn't have the right type." << std::endl;
                throw;
            }
        }
    }
    
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
    std::array<real,3> B0 = {0.0, 0.0, 0.0};
    real brms = 0.0;
    real ampl = 0.0;
    bool randomizeSeed = false;
    std::string initcond;

    /* Diagnostics and data I/O */
    int itWrite = 1;
    int itPrint = 1;

    bool writeSources = false;
    bool writeData = true;
    bool writeDiagnostics = true;

    bool overwriteSnapshots = false;

    /* Smoothing */
    bool filterDensity = false;
    bool filterVelocity = false;
    bool filterEfield = false;
    bool filterBfield = false;

    /* Debugging */
    bool verbose = false;
    bool throwOnCopyConstruct = false;

    /* Read configuration from input file */
    void read (const std::string& filename)
    {
        try
        {
            input.readFile (filename.c_str());
        }
        catch (const libconfig::FileIOException &fioex)
        {
            std::cerr << "I/O error while reading file." << std::endl;
            throw;
        }
        catch (const libconfig::ParseException &pex)
        {
            std::cerr << "Parse error at " << pex.getFile () << ":"
                      << pex.getLine () << " - " << pex.getError () << std::endl;
            throw;
        }

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
        readVariable ("initcond", initcond);

        /* Diagnostics and data I/O */
        readVariable ("itWrite", itWrite);
        readVariable ("itPrint", itPrint);

        readVariable ("writeSources", writeSources);
        readVariable ("writeData", writeData);
        readVariable ("writeDiagnostics", writeDiagnostics);

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

    /* Write configuration to output file */
    void write (const std::string& filename)
    {
        /* Append parameters */
        {
            const int precision = sizeof (real);
            output.getRoot ().add ("precision", Setting::TypeInt) = precision;
        }
        output.getRoot ().add ("nx", Setting::TypeInt) = vfpic::nx;
        output.getRoot ().add ("nz", Setting::TypeInt) = vfpic::nz;
        output.getRoot ().add ("npc", Setting::TypeInt) = vfpic::npc;
        output.getRoot ().add ("nthreads", Setting::TypeInt) = vfpic::nthreads;
        output.getRoot ().add ("alignment", Setting::TypeInt) = vfpic::alignment;

        /* Also append global constants */
        output.getRoot ().add ("mx", Setting::TypeInt) =  vfpic::mx;
        output.getRoot ().add ("mz", Setting::TypeInt) = vfpic::mz;
        output.getRoot ().add ("npar", Setting::TypeInt) = vfpic::npar;
        output.getRoot ().add ("mpar", Setting::TypeInt) = vfpic::mpar;

        try
        {
            output.writeFile (filename.c_str());
        }
        catch (const libconfig::FileIOException &fioex)
        {
            std::cerr << "I/O error while writing file: " << filename << std::endl;

            throw;
        }
    }
}