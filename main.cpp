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
#include <cstdio>
#include <iostream>
#include <libconfig.h++>
#include <random>
#include <thread>
#include <vector>

void initialCondition (GlobalVariables<real>& global)
{
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
    
    for (auto p = global.particles.begin (); p != global.particles.end (); ++p)
    {
        p->x = config::x0 + uniform (gen)*config::Lx;
        p->z = config::z0 + uniform (gen)*config::Lz;
        
        p->vx = config::cs0*normal (gen);
        p->vy = config::cs0*normal (gen);
        p->vz = config::cs0*normal (gen);
    }
    
    global.A = real (0);
    global.E = real (0);
    
    boundaryCondition (global.A);
    boundaryCondition (global.E);
    
    curl (global.B, global.A);
    
    boundaryCondition (global.B);
}

void iteration (std::array<GlobalVariables<real>,2>& global, Barrier& barrier, const int ithread, int niter)
{
    GlobalVariables<real>& global0 = global.at (0);

    LocalVectorFieldView<real> A0 (global0.A, ithread);
    LocalVectorFieldView<real> B0 (global0.B, ithread);
    LocalVectorFieldView<real> E0 (global0.E, ithread);
    
    LocalParticleArrayView<real> particles0 (global0.particles, ithread);

    GlobalVariables<real>& global1 = global.at (1);

    LocalVectorFieldView<real> A1 (global1.A, ithread);
    LocalVectorFieldView<real> B1 (global1.B, ithread);
    LocalVectorFieldView<real> E1 (global1.E, ithread);
    
    LocalParticleArrayView<real> particles1 (global1.particles, ithread);
    
    LocalVectorField<real> H, J;
    
    curl (H, A0);
    
    Deposit deposit (barrier, ithread);

    for (int it = 0; it < niter; ++it)
    {
        B0 = H;
        
        faraday (A0, E0);
        boundaryCondition (global0.A, barrier, ithread);
        
        curl (H, A0);
        
        B0 += H; B0 *= real (0.5);
        boundaryCondition (global0.B, barrier, ithread);
        
        drift (particles0);
        kick (particles0, global0.E, global0.B);
        
        deposit (global0);
        deposit (global0);
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
    
    initialCondition (global.at (0));
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
