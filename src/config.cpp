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
#include <sstream>

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
}

template <typename T>
class ConversionError: public std::exception
{
public:
    ConversionError (const T value): value (value) {}
    virtual const char *what () const noexcept
    {
        std::ostringstream stream;
        stream << "value = '" << value << "'";
        const std::string& str = stream.str ();
        return str.c_str ();
    }
private:
    const T value;
};

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

std::istream& operator>> (std::istream& is, ConfigParser& cfg)
{
    const std::string whitespace (" \t");
    std::string line, section;
    size_t pos;
    unsigned lineNumber = 0;
    while (std::getline (is, line))
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
            else throw ConfigParser::MissingBracket (lineNumber);
        }
        else
        {
            pos = line.find ('=');
            if (pos != line.npos)
            {
                // Extract value
                std::string value = line.substr (pos + 1);
                if (value.find ('=') != value.npos)
                {
                    throw ConfigParser::DoubleAssignment (lineNumber);
                }
                value.erase (0, value.find_first_not_of (whitespace));
                //                    if (value.empty ()) throw MissingValue (lineNumber);
                value.erase (value.find_last_not_of (whitespace) + 1);

                // Extract key
                std::string key = line.substr (0, pos);
                key.erase (0, key.find_first_not_of ("\t "));
                if (key.empty ())
                {
                    throw ConfigParser::MissingKey (lineNumber);
                }
                key.erase (key.find_last_not_of ("\t ") + 1);

                cfg.contents[section][key] = value;
            }
            else throw ConfigParser::NoAssignment (lineNumber);
        }
    }
    return is;
}

template <typename T>
T ConfigParser::get (const std::string& section, const std::string& key)
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
void ConfigParser::get (const std::string& section, const std::string& key, T& value)
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
void ConfigParser::set (const std::string& section, const std::string& key, const T value)
{
    contents[section][key] = toString (value);
}

std::ostream& operator<< (std::ostream& os, ConfigParser& parser)
{
    for (auto& section: parser.contents)
    {
        os << "[" << section.first << "]" << std::endl;
        for (auto& key: section.second)
        {
            os << key.first << " = " << key.second << std::endl;
        }
    }
    return os;
}

const char *ConfigParser::ParseError::what () const noexcept
{
    return msg.c_str ();
}
ConfigParser::MissingBracket::MissingBracket (const unsigned line)
{
    std::stringstream ss;
    ss << "Error at line " << line << ": Missing ']'";
    this->msg = ss.str ();
}
ConfigParser::DoubleAssignment::DoubleAssignment (const unsigned line)
{
    std::stringstream ss;
    ss << "Error at line " << line << ": Double assignment";
    this->msg = ss.str ();
}
ConfigParser::MissingKey::MissingKey (const unsigned line)
{
    std::stringstream ss;
    ss << "Error at line " << line << ": Key is missing";
    this->msg = ss.str ();
}
ConfigParser::MissingValue::MissingValue (const unsigned line)
{
    std::stringstream ss;
    ss << "Error at line " << line << ": Value is missing";
    this->msg = ss.str ();
}
ConfigParser::NoAssignment::NoAssignment (const unsigned line)
{
    std::stringstream ss;
    ss << "Error at line " << line << ": Not an assignment";
    this->msg = ss.str ();
}

const char *ConfigParser::NotFound::what () const noexcept
{
    return msg.c_str ();
}
ConfigParser::KeyNotFound::KeyNotFound (const std::string& key)
{
    this->msg = "Key '" + key + "' not found";
}
ConfigParser::SectionNotFound::SectionNotFound (const std::string& section)
{
    this->msg = "Section '" + section + "' not found";
}

std::istream& operator>> (std::istream& is, Config& cfg)
{
    using namespace config;

    ConfigParser& parser = cfg.parser;
    
    is >> parser;

    /* Grid */
    parser.get ("vfpic", "x0", x0);
    parser.get ("vfpic", "z0", z0);

    parser.get ("vfpic", "Lx", Lx);
    parser.get ("vfpic", "Lz", Lz);

    /* Time stepping */
    parser.get ("vfpic", "dt", dt);
    parser.get ("vfpic", "nt", nt);

    /* Physics */
    parser.get ("vfpic", "rho0", rho0);
    parser.get ("vfpic", "cs0", cs0);
    parser.get ("vfpic", "em", em);

    /* Shearing sheet */
    parser.get ("vfpic", "Omega", Omega);
    parser.get ("vfpic", "qshear", qshear);

    /* Initial condition */
    parser.get ("vfpic", "ikx", ikx);
    parser.get ("vfpic", "ikz", ikz);
    parser.get ("vfpic", "B0", B0);
    parser.get ("vfpic", "brms", brms);
    parser.get ("vfpic", "ampl", ampl);
    parser.get ("vfpic", "randomizeSeed", randomizeSeed);
    parser.get ("vfpic", "randomizePositions", randomizePositions);
    parser.get ("vfpic", "initcond", initcond);

    /* Diagnostics and data I/O */
    parser.get ("vfpic", "itWrite", itWrite);
    parser.get ("vfpic", "itPrint", itPrint);

    parser.get ("vfpic", "writeSources", writeSources);
    parser.get ("vfpic", "writeData", writeData);
    parser.get ("vfpic", "writeDiagnostics", writeDiagnostics);
    parser.get ("vfpic", "writeParticles", writeParticles);

    parser.get ("vfpic", "overwriteSnapshots", overwriteSnapshots);

    /* Smoothing */
    parser.get ("vfpic", "filterDensity", filterDensity);
    parser.get ("vfpic", "filterVelocity", filterVelocity);
    parser.get ("vfpic", "filterEfield", filterEfield);
    parser.get ("vfpic", "filterBfield", filterBfield);

    /* Debugging */
    parser.get ("vfpic", "verbose", verbose);
    parser.get ("vfpic", "throwOnCopyConstruct", throwOnCopyConstruct);

    /* Store precision */
    parser.set ("vfpic", "precision", sizeof (real));

    /* Append parameters */
    parser.set ("vfpic", "nx", vfpic::nx);
    parser.set ("vfpic", "nz", vfpic::nz);
    parser.set ("vfpic", "npc", vfpic::npc);
    parser.set ("vfpic", "nthreads", vfpic::nthreads);
    parser.set ("vfpic", "alignment", vfpic::alignment);

    /* Also append global constants */
    parser.set ("vfpic", "mx", vfpic::mx);
    parser.set ("vfpic", "mz", vfpic::mz);
    parser.set ("vfpic", "npar", vfpic::npar);
    parser.set ("vfpic", "mpar", vfpic::mpar);

    return is;
}
void Config::read (const std::string& filename)
{
    std::ifstream file (filename);
    file >> *this;
    file.close ();
}

void Config::write (const std::string& filename)
{
    std::ofstream file (filename);
    file << *this;
    file.close ();
}
void Config::print ()
{
    std::cout << *this;
}
std::ostream& operator<< (std::ostream& os, Config& cfg)
{
    os << cfg.parser;
    return os;
}
