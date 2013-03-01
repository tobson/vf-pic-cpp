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
#include "faraday.h"
#include "kick.h"
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

    LocalVectorFieldView<real> A2 (global.A2, ithread);
    LocalParticlesView particles2 (global.particles2, ithread);
    
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);

    BoundaryConditionsThreaded boundaryCondition (barrier, ithread);
    Deposit<vfpic::mpar> deposit (barrier, ithread);
    
    A2 = A;
    faraday (&A2, E, 0.5*dt);
    boundaryCondition (global.A2);
    curl (A2, &B); B += global.B0;
    particles2 = particles;
    kick (global.E, global.B, &particles2, 0.5*dt);
    deposit (particles2, &global.rho, &global.ruu);

    std::ostream& os = global.datafile;

    if (barrier.wait ())
    {
        os << global.A << global.A2 << global.B << global.E;
        if (config::writeParticles) os << global.particles << global.particles2;
        os << global.rho << global.ruu;
        os << global.grid;
        os.write (reinterpret_cast<const char *> (&it), sizeof (long long));
    }
    barrier.wait ();
}