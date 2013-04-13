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

#define LOOP(field, k, i) for (uint k = field::k1; k < field::k2; ++k) for (uint i = field::i1; i < field::i2; ++i)

void iteration (GlobalVariables& global, Diagnostics& diagnostics,
                Barrier& barrier, const uint ithread)
{
    using namespace config;
    using namespace vfpic;

    const real half = 0.5;
    const real Sdt = Sshear*dt;
    const real Sdt4 = 0.25*Sdt;
    
    typedef LocalScalarField<real> LocalField;

    /* Create local views of global data. Each thread gets its own chunk */
    LocalVectorFieldView<real> A (global.A, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);
    LocalVectorFieldView<real> E (global.E, ithread);

    LocalVectorFieldView<real> A2 (global.A2, ithread);
    LocalVectorFieldView<real> B2 (global.B2, ithread);
    LocalVectorFieldView<real> E2 (global.E2, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    LocalParticlesView particles  (global.particles , ithread);
    LocalParticlesView particles2 (global.particles2, ithread);

    /* These variables don't need to be global */
    NewLocalVectorField<real> D, D2;
    NewLocalVectorField<real> J, J2;

    /* Construct function objects */
    BoundaryConditionsThreaded boundCond (barrier, ithread);
    Deposit<mpar> deposit (barrier, ithread);
    Output output (barrier, ithread);
    Ohm<mz,mx> ohm;
    
    for (int it = 0; it < nt; ++it)
    {
        // Write out data
        if (itWrite > 0) if (it % itWrite == 0) output (global, it);

        /******************
         * Predictor step *
         ******************/

        // Advance vector potential from n-1/2 to n
        LOOP(LocalField, k,i) A.y (k,i) -= half*dt* E.y (k,i);
        LOOP(LocalField, k,i) A.z (k,i) -= half*dt* E.z (k,i);
        LOOP(LocalField, k,i) A.x (k,i) -= half*dt*(E.x (k,i) + Sshear*A.y (k,i));
        boundCond (global.A);

        // Compute magnetic field at n
        curl (A, &B); B += global.B0;
        boundCond (global.B);

        // Advance particle velocities from n-1/2 to n+1/2
        kick (global.E, global.B, &particles, dt);

        // Advance particle positions to n+1/2
        drift (&particles, half*dt);

        // Compute ion mass density and momentum density at n+1/2
        deposit (particles, &global.rho, &global.ruu);

        // Advance particle positions to n+1
        drift (&particles, half*dt);

        // Advance vector potential from n to n+1/2
        LOOP(LocalField, k,i) A.x (k,i) -= half*dt*(E.x (k,i) + Sshear*A.y (k,i));
        LOOP(LocalField, k,i) A.y (k,i) -= half*dt* E.y (k,i);
        LOOP(LocalField, k,i) A.z (k,i) -= half*dt* E.z (k,i);
        boundCond (global.A);

        // Compute magnetic field and curent at n+1/2
        curl (A, &B); B += global.B0;
        curlcurl (A, &J);

        // Compute electric field at n+1/2
        ohm (B, J, rho, ruu, &D);

        // Use trapezoidal rule to estimate electric field at n+1
        extrapolate (E, D, &E2);
        if (lshear) LOOP (LocalField, k,i) E2.x (k,i) += Sdt*(E.y (k,i) - D.y (k,i));
        boundCond (global.E2);

        /* Make copy of dynamice variables */
        particles2 = particles;

        /******************
         * Corrector step *
         ******************/

        // Advance vector potential from n+1/2 to n+1
        LOOP(LocalField, k,i) A2.y (k,i) = A.y (k,i) - half*dt* E2.y (k,i);
        LOOP(LocalField, k,i) A2.z (k,i) = A.z (k,i) - half*dt* E2.z (k,i);
        LOOP(LocalField, k,i) A2.x (k,i) = A.x (k,i) - half*dt*(E2.x (k,i) + Sshear*A2.y (k,i));
        boundCond (global.A2);

        // Compute magnetic field at n+1
        curl (A2, &B2); B2 += global.B0;
        boundCond (global.B2);

        // Advance particle velocities from n+1/2 to n+3/2
        kick (global.E2, global.B2, &particles2, dt);

        // Advance particle positions to n+3/2
        drift (&particles2, half*dt);

        // Compute ion mass density and momentum density at n+3/2
        deposit (particles2, &global.rho, &global.ruu);

        // Advance vector potential from n+1 to n+3/2
        LOOP (LocalField, k,i) A2.x (k,i) -= half*dt*(E2.x (k,i) + Sshear*A2.y (k,i));
        LOOP (LocalField, k,i) A2.y (k,i) -= half*dt* E2.y (k,i);
        LOOP (LocalField, k,i) A2.z (k,i) -= half*dt* E2.z (k,i);
        boundCond (global.A2);

        // Compute magnetic field and current at n+3/2
        curl (A2, &B2); B2 += global.B0;
        curlcurl (A2, &J2);

        // Compute electric field at n+3/2
        ohm (B2, J2, rho, ruu, &D2);

        // Average to get electric field at n+1
        average (D, D2, &E);
        if (lshear) LOOP (LocalField, k,i) E.x (k,i) += Sdt4*(D2.y (k,i) - D.y (k,i));
        boundCond (global.E);
    }

    /* Write out final data */
    output (global, nt);
}

void computeSelfConsistentElectricField (GlobalVariables& global, Barrier& barrier, const uint ithread)
{
    using namespace config;
    using namespace vfpic;
    
    const real half = 0.5;

    typedef LocalScalarField<real> LocalField;

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

    // Make sure boundary condition is set
    boundCond (global.A);

    // Compute magnetic field and current
    curl (A, &B); B += global.B0;
    curlcurl (A, &J);

    // Compute ion mass density and momentum density at n
    deposit (particles, &global.rho, &global.ruu);

    // Compute electric field at n
    ohm (B, J, rho, ruu, &E);

    // Evolve vector potential back in time by a half step
    LOOP(LocalField, k,i) A.x (k,i) += half*dt*(E.x (k,i) + Sshear*A.y (k,i));
    LOOP(LocalField, k,i) A.y (k,i) += half*dt* E.y (k,i);
    LOOP(LocalField, k,i) A.z (k,i) += half*dt* E.z (k,i);

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
