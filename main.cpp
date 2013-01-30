//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "config.h"
#include "global.h"
#include "grid.h"
#include "particles.h"

#include <iostream>
#include <libconfig.h++>
#include <thread>
#include <vector>

struct Fields
{
    GlobalScalarField<real> A, E;
};

class Iteration
{
public:
    void operator() (Fields& fields, GlobalParticleArray<real>& particles, Barrier& barrier, int ithread)
    {
    }
};

int main(int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    try
    {
        config::read (srcdir + "/config.in");
        global::computeVariables();
        config::write (srcdir + "/config.out");
    }
    catch (const libconfig::ConfigException& e)
    {
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what () << std::endl;
        return 1;
    }
    
    Barrier barrier (global::nthreads);
    Fields fields;
    Grid grid;
    GlobalParticleArray<real> particles;
    Iteration iteration;
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < global::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (fields), std::ref (particles), std::ref (barrier), ithread);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
