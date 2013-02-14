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
#include "drift.h"
#include "faraday.h"
#include "global.h"
#include "grid.h"
#include "initial-condition.h"
#include "kick.h"
#include "math-helpers.h"
#include "ohm.h"
#include "particles.h"
#include "variables.h"
#include "vector-field.h"

#include <array>
#include <cstdio>
#include <iostream>
#include <libconfig.h++>
#include <random>
#include <thread>
#include <vector>

//template <typename T>
//struct GlobalVariables
//{
//    NewGlobalVectorField<T> A, A2;
//    NewGlobalParticles<T> particles, particles2;
//    NewGlobalVectorField<T> E, B;
//    NewGlobalScalarField<T> rho; NewGlobalVectorField<T> ruu;
//};

void iteration (GlobalVariables<real>& global, Barrier& barrier, const int ithread, int niter)
{
    using config::dt;

    LocalVectorFieldView<real> A  (global.A , ithread);
    LocalVectorFieldView<real> A2 (global.A2, ithread);
    
    LocalParticlesView<real> particles  (global.particles , ithread);
    LocalParticlesView<real> particles2 (global.particles2, ithread);
    
    NewLocalVectorField<real> D, D2;
    NewLocalVectorField<real> H, H2;
    NewLocalVectorField<real> J;

    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);
    
    BoundaryCondition<real> boundCond (barrier, ithread);
    Deposit<real,vfpic::mpar> deposit (barrier, ithread);
    Ohm<real,vfpic::mz,vfpic::mx> ohm;
    
    curl (A, &H2);
    
    for (int it = 0; it < niter; ++it)
    {
        /* Predictor step */
        
        faraday (&A, E, dt);
        boundCond (global.A);
        
        curl (A, &H);
        curlcurl (A, &J);
                
        average (H2, H, &B);

        /* It would be nice if boundCond::operator()
           would be a variadic function */
        boundCond (global.E);
        boundCond (global.B);
        
        kick (global.E, global.B, &particles, dt);
        
        drift (&particles, 0.5*dt);
        
        deposit (particles, &global.rho, &global.ruu);
        
        drift (&particles, 0.5*dt);

        ohm (H, J, rho, ruu, &D);
        
        extrapolate (E, D, &E);
        
        /* Save for next time step */
        H2 = H;
        
        /* Make copy of dynamice variables */
        A2 = A;
        particles2 = particles;

        /* Corrector step */

        faraday (&A2, E, dt);
        boundCond (global.A2);

        curl (A2, &H);
        curlcurl (A2, &J);

        average (H2, H, &B);

        boundCond (global.E);
        boundCond (global.B);

        kick (global.E, global.B, &particles2, dt);
        
        drift (&particles2, 0.5*dt);
        
        deposit (particles2, &global.rho, &global.ruu);
        
        ohm (H, J, rho, ruu, &D2);

        average (D, D2, &E);
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
    GlobalVariables<real> global;
    
    initialCondition (grid, &global);
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
