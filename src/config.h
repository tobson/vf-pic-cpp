//
//  config.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__config__
#define __vf_pic__config__

#include <array>
#include <map>
#include <fstream>
#include <string>

#include "param.h"

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
    //extern std::array<real,3> B0;
    extern real B0;
    extern real brms;
    extern real ampl;
    extern bool randomizeSeed;
    extern bool randomizePositions;
    extern std::string initcond;

    /* Diagnostics and data I/O */
    extern int itWrite;
    extern int itPrint;

    extern bool writeSources;
    extern bool writeData;
    extern bool writeDiagnostics;
    extern bool writeParticles;

    extern bool overwriteSnapshots;

    /* Smoothing */
    extern bool filterDensity;
    extern bool filterVelocity;
    extern bool filterEfield;
    extern bool filterBfield;

    /* Debugging */
    extern bool verbose;
    extern bool throwOnCopyConstruct;
}

class ConfigParser
{
private:
    template <typename T>
    using stringmap = std::map<std::string,T>;
    stringmap<stringmap<std::string>> contents;
public:
    template <typename T>
    T get (const std::string&, const std::string&);
    template <typename T>
    void get (const std::string&, const std::string&, T&);
    template <typename T>
    void set (const std::string&, const std::string&, const T);
    friend std::istream& operator>> (std::istream&, ConfigParser&);
    friend std::ostream& operator<< (std::ostream&, ConfigParser&);
    class ParseError: public std::exception
    {
    public:
        virtual const char *what () const noexcept;
    protected:
        std::string msg;
    };
    struct MissingBracket: public ParseError
    {
        explicit MissingBracket (const unsigned);
    };
    struct DoubleAssignment: public ParseError
    {
        explicit DoubleAssignment (const unsigned);
    };
    struct MissingKey: public ParseError
    {
        explicit MissingKey (const unsigned);
    };
    struct MissingValue: public ParseError
    {
        explicit MissingValue (const unsigned);
    };
    struct NoAssignment: public ParseError
    {
        explicit NoAssignment (const unsigned);
    };
    class NotFound: public std::exception
    {
    public:
        virtual const char *what () const noexcept;
    protected:
        std::string msg;
    };
    struct KeyNotFound: public NotFound
    {
        explicit KeyNotFound (const std::string&);
    };
    struct SectionNotFound: public NotFound
    {
        explicit SectionNotFound (const std::string&);
    };
};

class Config
{
public:
    void read (const std::string&);
    friend std::istream& operator>> (std::istream&, Config&);
    void write (const std::string&);
    void print ();
    friend std::ostream& operator<< (std::ostream&, Config&);
private:
    ConfigParser parser;
};


#endif /* defined(__vf_pic__config__) */
