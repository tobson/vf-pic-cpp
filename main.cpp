//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "boundaries.h"
#include "config.h"
#include "deposit.h"
#include "diffops.h"
#include "drift-kick.h"
#include "faraday.h"
#include "global.h"
#include "global-variables.h"
#include "grid.h"
#include "particles.h"
#include "vector-field.h"

#include <array>
#include <iostream>
#include <libconfig.h++>
#include <thread>
#include <cstdio>
#include <vector>


void iteration (std::array<GlobalVariables<real>,2>& global, Barrier& barrier, const int ithread, int niter)
{
    GlobalVariables<real>& global0 = global.at (0);

    VectorPair<real> A0 (global0.A, ithread);
    VectorPair<real> B0 (global0.B, ithread);
    VectorPair<real> E0 (global0.E, ithread);
    
    LocalParticleArrayView<real> particles0 (global0.particles, ithread);

    GlobalVariables<real>& global1 = global.at (1);

    VectorPair<real> A1 (global1.A, ithread);
    VectorPair<real> B1 (global1.B, ithread);
    VectorPair<real> E1 (global1.E, ithread);
    
    LocalParticleArrayView<real> particles1 (global1.particles, ithread);
    
    LocalVectorField<real> H1, J;
    
    curl (H1, A1.local);
    
    Deposit deposit (barrier, ithread);

    for (int it = 0; it < niter; ++it)
    {
        B1.local = H1;
        
        faraday (A1.local, E1.local);
        boundaryCondition (A1, barrier);
        
        curl (H1, A1.local);
        curlcurl(J, A1.local);
        
        B1.local += H1; B1.local *= real (0.5);
        boundaryCondition (B1, barrier);
        
        drift (particles1);
        kick (particles1, E1.global, B1.global);
        
        deposit (particles1, global0.rho1, global0.U);
        deposit (particles1, global0.rho1, global0.U);
    }
    printf ("Hi, I'm thread %d!\n", ithread);
}

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
    std::array<GlobalVariables<real>,2> global;
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
