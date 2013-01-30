//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "config.h"
#include "faraday.h"
#include "global.h"
#include "grid.h"
#include "particles.h"
#include "vector-field.h"

#include <iostream>
#include <libconfig.h++>
#include <thread>
#include <cstdio>
#include <vector>

struct System
{
    GlobalVectorField<real> A, E;
    GlobalParticleArray<real> particles;
};

class Iteration
{
public:
    void operator() (System(& system)[2], Barrier& barrier, int ithread, int niter)
    {
        LocalVectorFieldView<real> A1 (system[0].A, ithread);
        LocalVectorFieldView<real> E1 (system[0].E, ithread);

        LocalParticleArrayView<real> particles1 (system[0].particles, ithread);

        LocalVectorFieldView<real> A2 (system[1].A, ithread);
        LocalVectorFieldView<real> E2 (system[1].E, ithread);

        LocalParticleArrayView<real> particles2 (system[1].particles, ithread);
        
        faraday (A1, E1);

        printf ("Hi, I'm thread %d!\n", ithread);
    }
};

int main (int argc, const char * argv[])
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
    Grid grid;
    Iteration iteration;
    System system[2];
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < global::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (system), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
