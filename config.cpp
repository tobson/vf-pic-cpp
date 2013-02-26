//
//  config.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <libconfig.h++>
#include <yaml-cpp/yaml.h>

#include "global.h"

namespace config
{
    template <typename T>
    std::string toString_internal (const T& value)
    {
        std::ostringstream stream;
        stream << std::fixed << std::scientific << std::boolalpha;
        stream << std::setprecision (std::numeric_limits<T>::digits10 + 1);
        stream << value;
        return stream.str ();
    }
    template <>
    std::string toString_internal (const std::string& value)
    {
        std::string str = value;
        return str;
    }
    template <>
    std::string toString_internal (const char *const& value)
    {
        std::string str (value);
        return str;
    }
    template <typename T>
    std::string toString (const T& value)
    {
        return toString_internal <typename std::decay<const T>::type> (value);
    }
    template <typename T>
    T fromString (const std::string& str)
    {
        T value;
        std::istringstream stream (str);
        stream >> std::fixed >> std::scientific >> std::boolalpha;
        stream >> std::setprecision (std::numeric_limits<T>::digits10 + 1);
        if ((stream >> value).fail () || !(stream >> std::ws).eof ())
        {
            throw ConversionError<std::string> (str);
        }
        return value;
    }
    template <>
    std::string fromString (const std::string& str)
    {
        std::string value = str;
        return value;
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

    void Config::parse (const char *filename)
    {
        std::ifstream file (filename);
        parse (file);
        file.close ();
    }

    void Config::parse (std::ifstream& file)
    {
        const std::string whitespace (" \t");
        std::string line, section;
        size_t pos;
        unsigned lineNumber = 0;
        while (std::getline (file, line))
        {
            ++lineNumber;

            // Remove comments
            pos = line.find_first_of (";#");
            if (pos != line.npos) line.erase (pos);

            // Remove leading white space
            pos = line.find_first_not_of (whitespace);
            if (pos != line.npos)
            {
                line.erase (0, pos);
            }
            else continue; // Ignore empty lines

            if (line[0] == '[')
            {
                pos = line.find (']');
                if (pos != line.npos)
                {
                    section = line.substr (1, pos - 1);
                }
                else throw MissingBracket (lineNumber);
            }
            else
            {
                pos = line.find ('=');
                if (pos != line.npos)
                {
                    // Extract value
                    std::string value = line.substr (pos + 1);
                    if (value.find ('=') != value.npos) throw DoubleAssignment (lineNumber);
                    value.erase (0, value.find_first_not_of (whitespace));
//                    if (value.empty ()) throw MissingValue (lineNumber);
                    value.erase (value.find_last_not_of (whitespace) + 1);

                    // Extract key
                    std::string key = line.substr (0, pos);
                    key.erase (0, key.find_first_not_of ("\t "));
                    if (key.empty ()) throw MissingKey (lineNumber);
                    key.erase (key.find_last_not_of ("\t ") + 1);
                    std::cout << "key = " << key << std::endl;

                    contents[section][key] = value;
                }
                else
                {
                    throw NoAssignment (lineNumber);
                }
            }
        }
    }

    template <typename T>
    T Config::get (const std::string& section, const std::string& key)
    {
        auto s = contents.find (section);
        if (s != contents.end ())
        {
            auto keys = s->second;
            auto k = keys.find (key);
            if (k != keys.end ())
            {
                T value = fromString<T> (k->second);

                // Update config
                contents[section][key] = toString (value);

                return value;
            }
            else
            {
                throw KeyNotFound (key);
            }
        }
        else
        {
            throw SectionNotFound (section);
        }
    }
    
    template <typename T>
    void Config::get (const std::string& section, const std::string& key, T& value)
    {
        try
        {
            value = get<T> (section, key);
        }
        catch (NotFound& e)
        {
            contents[section][key] = toString (value);
        }
    }

    template <typename T>
    void Config::set (const std::string& section, const std::string& key, const T value)
    {
        contents[section][key] = toString (value);
    }

    void Config::save (const char *filename)
    {
        for (auto c = contents.begin (); c != contents.end (); ++c)
        {
            std::string section = c->first;
            stringmap<std::string> keys = c->second;
            std::cout << "[" << section << "]" << std::endl;
            for (auto k = keys.begin (); k != keys.end (); ++k)
            {
                std::cout << k->first << " = " << k->second << std::endl;
            }
        }
        std::ofstream file (filename);
        for (auto& section: contents)
        {
            file << "[" << section.first << "]" << std::endl;
            for (auto& key: section.second)
            {
                file << key.first << " = " << key.second << std::endl;
            }
        }
        file.close ();
    }

    const char *Config::ParseError::what () const noexcept
    {
        return msg.c_str ();
    }
    Config::MissingBracket::MissingBracket (const unsigned line)
    {
        std::stringstream ss;
        ss << "Error at line " << line << ": Missing ']'";
        this->msg = ss.str ();
    }
    Config::DoubleAssignment::DoubleAssignment (const unsigned line)
    {
        std::stringstream ss;
        ss << "Error at line " << line << ": Double assignment";
        this->msg = ss.str ();
    }
    Config::MissingKey::MissingKey (const unsigned line)
    {
        std::stringstream ss;
        ss << "Error at line " << line << ": Key is missing";
        this->msg = ss.str ();
    }
    Config::MissingValue::MissingValue (const unsigned line)
    {
        std::stringstream ss;
        ss << "Error at line " << line << ": Value is missing";
        this->msg = ss.str ();
    }
    Config::NoAssignment::NoAssignment (const unsigned line)
    {
        std::stringstream ss;
        ss << "Error at line " << line << ": Not an assignment";
        this->msg = ss.str ();
    }

    const char *Config::NotFound::what () const noexcept
    {
        return msg.c_str ();
    }
    Config::KeyNotFound::KeyNotFound (const std::string& key)
    {
        this->msg = "Key '" + key + "' not found";
    }
    Config::SectionNotFound::SectionNotFound (const std::string& section)
    {
        this->msg = "Section '" + section + "' not found";
    }

    template <typename T>
    ConversionError<T>::ConversionError (const T value): value (value) {}

    template <typename T>
    const char *ConversionError<T>::what () const noexcept
    {
        std::ostringstream stream;
        stream << "value = '" << value << "'";
        const std::string& str = stream.str ();
        return str.c_str ();
    }

    void read (const std::string& srcdir)
    {
        Config cfg;
        std::string filename;
        
        filename = srcdir + "/input.cfg";
        cfg.parse (filename.c_str ());

        /* Grid */
        cfg.get ("vfpic", "x0", x0);
        cfg.get ("vfpic", "z0", z0);

        cfg.get ("vfpic", "Lx", Lx);
        cfg.get ("vfpic", "Lz", Lz);

        /* Time stepping */
        cfg.get ("vfpic", "dt", dt);
        cfg.get ("vfpic", "nt", nt);

        /* Physics */
        cfg.get ("vfpic", "rho0", rho0);
        cfg.get ("vfpic", "cs0", cs0);
        cfg.get ("vfpic", "em", em);

        /* Shearing sheet */
        cfg.get ("vfpic", "Omega", Omega);
        cfg.get ("vfpic", "qshear", qshear);

        /* Initial condition */
        cfg.get ("vfpic", "ikx", ikx);
        cfg.get ("vfpic", "ikz", ikz);
        cfg.get ("vfpic", "B0", B0);
        cfg.get ("vfpic", "brms", brms);
        cfg.get ("vfpic", "ampl", ampl);
        cfg.get ("vfpic", "randomizeSeed", randomizeSeed);
        cfg.get ("vfpic", "randomizePositions", randomizePositions);
        cfg.get ("vfpic", "initcond", initcond);

        /* Diagnostics and data I/O */
        cfg.get ("vfpic", "itWrite", itWrite);
        cfg.get ("vfpic", "itPrint", itPrint);

        cfg.get ("vfpic", "writeSources", writeSources);
        cfg.get ("vfpic", "writeData", writeData);
        cfg.get ("vfpic", "writeDiagnostics", writeDiagnostics);
        cfg.get ("vfpic", "writeParticles", writeParticles);

        cfg.get ("vfpic", "overwriteSnapshots", overwriteSnapshots);

        /* Smoothing */
        cfg.get ("vfpic", "filterDensity", filterDensity);
        cfg.get ("vfpic", "filterVelocity", filterVelocity);
        cfg.get ("vfpic", "filterEfield", filterEfield);
        cfg.get ("vfpic", "filterBfield", filterBfield);

        /* Debugging */
        cfg.get ("vfpic", "verbose", verbose);
        cfg.get ("vfpic", "throwOnCopyConstruct", throwOnCopyConstruct);

        /* Store precision */
        cfg.set ("vfpic", "precision", sizeof (real));

        /* Append parameters */
        cfg.set ("vfpic", "nx", vfpic::nx);
        cfg.set ("vfpic", "nz", vfpic::nz);
        cfg.set ("vfpic", "npc", vfpic::npc);
        cfg.set ("vfpic", "nthreads", vfpic::nthreads);
        cfg.set ("vfpic", "alignment", vfpic::alignment);

        /* Also append global constants */
        cfg.set ("vfpic", "mx", vfpic::mx);
        cfg.set ("vfpic", "mz", vfpic::mz);
        cfg.set ("vfpic", "npar", vfpic::npar);
        cfg.set ("vfpic", "mpar", vfpic::mpar);

        filename = srcdir + "/output.cfg";
        cfg.save (filename.c_str ());

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
