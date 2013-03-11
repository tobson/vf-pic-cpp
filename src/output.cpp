//
//  output.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/15/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "output.h"

#include "boundaries.h"
#include "deposit.h"
#include "diffops.h"
#include "drift.h"
#include "faraday.h"
#include "ohm.h"

Output::Output (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread)
{
}

void Output::operator() (GlobalVariables& global, const long long it)
{
    using config::dt;

    const LocalVectorFieldView<real> A (global.A, ithread);
    const LocalVectorFieldView<real> E (global.E, ithread);
    const LocalParticlesView particles (global.particles, ithread);

    LocalParticlesView particles2 (global.particles2, ithread);
    
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    BoundaryConditionsThreaded boundaryCondition (barrier, ithread);
    Deposit<vfpic::mpar> deposit (barrier, ithread);
    
    // Magnetic field at n-1/2
    curl (A, &B); B += global.B0;
    boundaryCondition (global.B);
    // Evolve positions backward in time to n-1/2
    particles2 = particles;
    drift (&particles2, -0.5*dt);
    // Compute sources at n-1/2
    deposit (particles2, &global.rho, &global.ruu);

    std::ostream& os = global.datafile;

    if (barrier.wait ())
    {
        // Note: Electric field is at n, everything else at n-1/2
        os << global.A << global.B << global.E;
        if (config::writeParticles) os << global.particles2;
        os << global.rho << global.ruu;
        os << global.grid;
        os.write (reinterpret_cast<const char *> (&it), sizeof (long long));
        os.flush ();
        std::cout << "it = " << it << ": Snapshot written" << std::endl;
    }
    barrier.wait ();
}