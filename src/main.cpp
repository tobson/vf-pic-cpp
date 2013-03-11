//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <array>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stack>
#include <thread>
#include <vector>

#include "barrier.h"
#include "boundaries.h"
#include "config.h"
#include "deposit.h"
#include "diagnostics.h"
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

void iteration (GlobalVariables& global, Diagnostics& diagnostics,
                Barrier& barrier, const int ithread)
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

    LocalParticlesView particles  (global.particles , ithread);
    LocalParticlesView particles2 (global.particles2, ithread);

    /* These variables don't need to be global */
    NewLocalVectorField<real> D, D2;
    NewLocalVectorField<real> H, H2;
    NewLocalVectorField<real> J;

    /* Construct function objects */
    BoundaryConditionsThreaded boundCond (barrier, ithread);
    Deposit<vfpic::mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<vfpic::mz,vfpic::mx> ohm;
    
    curl (A, &H2); H2 += global.B0;

    for (int it = 0; it < nt; ++it)
    {
        // Write out data
        if (it % itWrite == 0)
        {
            if (ithread == 0) std::cout << "it = " << it << std::endl;
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
        
        // Diagnostics at n+1/2
        diagnostics (global, H, barrier, ithread);
        
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

void computeSelfConsistentElectricField (GlobalVariables& global, Barrier& barrier, const int ithread)
{
    using namespace config;
    using namespace vfpic;
    
    /* Create local views of global data. Each thread gets its own chunk */
    const LocalVectorFieldView<real> A  (global.A , ithread);
    LocalVectorFieldView<real> A2 (global.A2, ithread);

    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    const LocalParticlesView particles  (global.particles , ithread);
    LocalParticlesView particles2 (global.particles2, ithread);

    /* These variables don't need to be global */
    NewLocalVectorField<real> D, J;

    /* Construct function objects */
    BoundaryConditionsThreaded boundCond (barrier, ithread);
    Deposit<mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<mz,mx> ohm;

    // Make copy of dynamic variables
    for (;;)
    {
        A2 = A;
        particles2 = particles;

        // Evolve copy of vector potential to n+1/2 first and then average to get it at n
        faraday (&A2, E, dt);
        average (A, A2, &A2);
        boundCond (global.A2);

        // Compute magnetic field and current at n
        curl (A2, &B); B += global.B0;
        curlcurl (A2, &J);

        // Get velocities at n
        kick (global.E, global.B, &particles2, 0.5*dt);

        // Compute ion mass density and momentum density at n
        deposit (particles2, &global.rho, &global.ruu);

        // Compute electric field at n
        ohm (B, J, rho, ruu, &D);

        // Difference of old and new electric field
        A2 = E; A2 -= D; A2 *= real (0.5);
        // Average of old and new electric field
        average (E, D, &B);
        if (barrier.wait())
        {
            const real relerr = global.A2.rms ()/global.B.rms();
            global.A2[0](0,0) = relerr;
        }
        barrier.wait ();
        const real threshold = std::numeric_limits<real>::epsilon();
        std::cout << "relerr = " << global.A2[0](0,0) << ", threshold = " << threshold << std::endl;

        if (global.A2[0](0,0) < threshold) break;

        // Update electric field
        E = D;
        boundCond (global.E);
    }
}

int main (int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    Config cfg;
    cfg.read (srcdir + "/problem.cfg");
    cfg.write (srcdir + "/output.cfg");
    cfg.print ();

    vfpic::computeVariables();

    Barrier barrier (vfpic::nthreads);
    GlobalVariables global;
    Diagnostics diagnostics (srcdir + "/diag.txt");

    global.datafile.open (srcdir + "/var.dat", std::ios::binary);
    
    std::cout << "# of hardware threads: " << std::thread::hardware_concurrency()
              << std::endl;
    
    // Initialize
    initialCondition (&global);

    // Compute self-consistent electric field
    if (true) // if (config::iterateInitialElectricField)
    {
        std::vector<std::thread> threads;
        for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
        {
            std::thread thread (computeSelfConsistentElectricField, std::ref (global), std::ref (barrier), ithread);
            threads.push_back (std::move (thread));
        }
        for (auto thread = threads.begin (); thread != threads.end (); ++thread)
        {
            thread->join ();
        }
    }

    // Evolve in time
    std::vector<std::thread> threads;
    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (diagnostics), std::ref (barrier), ithread);
        threads.push_back (std::move (thread));
    }
    for (auto thread = threads.begin (); thread != threads.end (); ++thread)
    {
        thread->join ();
    }

    global.datafile.close ();

    std::cout << "Done!" << std::endl;

    return 0;
}
