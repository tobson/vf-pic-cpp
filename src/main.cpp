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
                Barrier& barrier, const uint ithread)
{
    using namespace config;
    using namespace vfpic;

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
    Deposit<mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<mz,mx> ohm;
    
    curl (A, &H2); H2 += global.B0;
    barrier.wait ();

    for (int it = 0; it < nt; ++it)
    {
        // Write out data
        if (itWrite > 0) if (it % itWrite == 0) output (global, it);

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
        boundCond (global.B);

        // Advance particle velocities from n-1/2 to n+1/2
        kick (global.E, global.B, &particles, dt);

        // Advance particle positions to n+1/2
        drift (&particles, 0.5*dt);

        // Compute ion mass density and momentum density at n+1/2
        deposit (particles, &global.rho, &global.ruu);
        
        // Diagnostics at n+1/2
        if (itDiag > 0)
            if (it % itDiag == 0) diagnostics (global, H, it, barrier, ithread);

        // Advance particle positions to n+1
        drift (&particles, 0.5*dt);

        // Compute electric field at n+1/2
        ohm (H, J, rho, ruu, &D);

        // Use trapezoidal rule to estimate electric field at n+1
        extrapolate (E, D, &E);
        boundCond (global.E);
        
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
        boundCond (global.E);
    }

    /* Write out final data */
    output (global, nt);
}

void computeSelfConsistentElectricField (GlobalVariables& global, Barrier& barrier, const uint ithread)
{
    using namespace config;
    using namespace vfpic;
    
    /* Create local views of global data. Each thread gets its own chunk */
    LocalVectorFieldView<real> A (global.A, ithread);
    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    LocalParticlesView particles (global.particles, ithread);

    /* These variables don't need to be global */
    NewLocalVectorField<real> D, J;

    /* Construct function objects */
    BoundaryConditionsThreaded boundCond (barrier, ithread);
    Deposit<mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<mz,mx> ohm;

    // Assume particle positions, particle velocities, and vector potential
    // are all given at the same time.

    // Compute magnetic field and current
    curl (A, &B); B += global.B0;
    curlcurl (A, &J);

    // Compute ion mass density and momentum density at n
    deposit (particles, &global.rho, &global.ruu);

    // Compute electric field at n
    ohm (B, J, rho, ruu, &E);

    // Evolve vector potential back in time by a half step
    faraday (&A, E, -0.5*dt);

    // Set boundary conditions
    boundCond (global.A);
    boundCond (global.E);
    boundCond (global.B);

    // Evolve particle velocities back in time by a half step
    kick (global.E, global.B, &particles, -0.5*dt);
}

int main (int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    Config cfg;
    cfg.read (srcdir + "/problem.cfg");
    cfg.write (srcdir + "/output.cfg");

    vfpic::computeVariables();

    Barrier barrier (vfpic::nthreads);
    GlobalVariables global;
    Diagnostics diagnostics (srcdir + "/diag.txt");

    global.datafile.open (srcdir + "/var.dat", std::ios::binary);
    
    std::cout << "# of hardware threads: "
              << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Larmor radius: " << config::cs0/(config::em*config::B0) << std::endl;
    std::cout << "Skin depth: " << 1.0/(config::em*sqrt (config::rho0)) << std::endl;
    
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
