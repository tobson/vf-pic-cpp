//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "config.h"
#include "diffops.h"
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
    GlobalVectorField<real> A, B, E;
    GlobalParticleArray<real> particles;
};

class Iteration
{
public:
    void operator() (System(& system)[2], Barrier& barrier, int ithread, int niter)
    {
        System& system1 = system[0];
        
        LocalVectorFieldView<real> A1 (system1.A, ithread);
        LocalVectorFieldView<real> B1 (system1.B, ithread);
        LocalVectorFieldView<real> E1 (system1.E, ithread);

        LocalParticleArrayView<real> particles1 (system1.particles, ithread);

        System& system2 = system[1];

        LocalVectorFieldView<real> A2 (system2.A, ithread);
        LocalVectorFieldView<real> B2 (system2.B, ithread);
        LocalVectorFieldView<real> E2 (system2.E, ithread);

        LocalParticleArrayView<real> particles2 (system2.particles, ithread);
        
        curl (H1, A1);

        for (int it = 0; it < niter; ++it)
        {
            B1 = H1;
            faraday (A1, E1);
            curl (H1, A1);
            B1 += H1; B1 *= real (0.5);
        }
        printf ("Hi, I'm thread %d!\n", ithread);
    }
    LocalVectorField<real> H1, J;
};

int main (int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    try
    {
        config::read (srcdir + "/config.in");
        vfpic::computeVariables();
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
    
    Barrier barrier (vfpic::nthreads);
    Grid grid;
    Iteration iteration;
    System system[2];
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (system), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
