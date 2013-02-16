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
#include "initial-condition.h"
#include "kick.h"
#include "math-helpers.h"
#include "ohm.h"
#include "output.h"
#include "particles.h"
#include "variables.h"
#include "vector-field.h"

#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <random>
#include <stack>
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

void iteration (GlobalVariables<real>& global, Barrier& barrier, const int ithread)
{
    using config::dt;
    using config::nt;
    using config::itWrite;

    /* Create local views of global data. Each thread gets its own chunk */
    LocalVectorFieldView<real> A  (global.A , ithread);
    LocalVectorFieldView<real> A2 (global.A2, ithread);
    
    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    LocalParticlesView<real> particles  (global.particles , ithread);
    LocalParticlesView<real> particles2 (global.particles2, ithread);

    /* These variables don't need to be global */
    NewLocalVectorField<real> D, D2;
    NewLocalVectorField<real> H, H2;
    NewLocalVectorField<real> J;

    /* Construct function objects */
    BoundaryCondition<real> boundCond (barrier, ithread);
    Deposit<real,vfpic::mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<real,vfpic::mz,vfpic::mx> ohm;
    
    curl (A, &H2); H2 += global.B0;

    for (int it = 0; it < nt; ++it)
    {
        // Write out data
        if (it % itWrite == 0)
        {
            printf ("it = %d\n", it);
            output (global, it);
        }

        /******************
         * Predictor step *
         ******************/

        // Advance vector potential from n-1/2 to n+1/2
        faraday (&A, E, dt);
        boundCond (global.A);

        // Compute magnetic field and current at n+1/2
        curl (A, &H); H += global.B0;
        curlcurl (A, &J);

        // Compute magnetic field at n
        average (H2, H, &B);

        /* It would be nice if boundCond::operator()
           would be a variadic function */
        // Set boundary condition, needed for faraday() and kick()
        boundCond (global.E);
        boundCond (global.B);

        // Advance particle velocities from n-1/2 to n+1/2
        kick (global.E, global.B, &particles, dt);

        // Advance particle positions to n+1/2
        drift (&particles, 0.5*dt);

        // Compute ion mass density and momentum density at n+1/2
        deposit (particles, &global.rho, &global.ruu);
        
        // Advance particle positions to n+1
        drift (&particles, 0.5*dt);

        // Compute electric field at n+1/2
        ohm (H, J, rho, ruu, &D);

        // Use trapezoidal rule to estimate electric field at n+1
        extrapolate (E, D, &E);
        
        /* Save magnetic field at n+1/2 for the next time step */
        H2 = H;
        
        /* Make copy of dynamice variables */
        A2 = A;
        particles2 = particles;

        /******************
         * Corrector step *
         ******************/

        // Advance vector potential to n+3/2
        faraday (&A2, E, dt);
        boundCond (global.A2);

        // Compute magnetic field and current at n+3/2
        curl (A2, &H); H += global.B0;
        curlcurl (A2, &J);

        // Compute magnetic field at n+1
        average (H2, H, &B);

        // Set boundary condition, needed for faraday() and kick()
        boundCond (global.E);
        boundCond (global.B);

        // Advance particle velocities from n+1/2 to n+3/2
        kick (global.E, global.B, &particles2, dt);
        
        // Advance particle positions to n+3/2
        drift (&particles2, 0.5*dt);
        
        // Compute ion mass density and momentum density at n+3/2
        deposit (particles2, &global.rho, &global.ruu);
        
        // Compute electric field at n+3/2
        ohm (H, J, rho, ruu, &D2);

        // Average to get electric field at n+1
        average (D, D2, &E);
    }

    /* Write out final data */
    output (global, nt);
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
    GlobalVariables<real> global;

    global.datafile.open (srcdir + "/var.dat");
    
    initialCondition (&global);

    /* Iterate */
    std::vector<std::thread> threads;
    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (barrier), ithread);
        threads.push_back (std::move (thread));
    }
    for (auto thread = threads.begin (); thread != threads.end (); ++thread)
    {
        thread->join ();
    }

    global.datafile.close ();

    printf ("Done!\n");

    return 0;
}
