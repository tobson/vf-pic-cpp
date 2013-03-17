//
//  noise.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 3/1/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <random>

#include "../boundaries.h"
#include "../variables.h"

void initialCondition (GlobalVariables *global)
{
    using namespace config;
    using namespace vfpic;
    
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
        
    GlobalVectorField<real>& A = global->A;
    GlobalParticles& particles = global->particles;
    
    BoundaryConditions boundCond;
    
    if (randomizePositions)
    {
        for (Particle *p = particles.begin (); p != particles.end (); ++p)
        {
            p->x = x0 + uniform (gen)*Lx;
            p->z = z0 + uniform (gen)*Lz;
        }
    }
    else
    {
        int exponent = int (log2 (real (npc)));
        if (int (pow (2.0, real (exponent))) != npc)
        {
            throw std::runtime_error ("'npc' must be a power of two");
        }
        const int npx = nx*int (pow (2.0, exponent/2));
        const int npz = nz*int (pow (2.0, exponent - exponent/2));
        for (int k = 0; k < npz; ++k)
        for (int i = 0; i < npx; ++i)
        {
            particles[k*npx + i].x = x0 + (real (i) - 0.5)*Lx/real (npx);
            particles[k*npx + i].z = z0 + (real (k) - 0.5)*Lz/real (npz);
        }
    }
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vx = cs0*normal (gen);
        p->vy = cs0*normal (gen);
        p->vz = cs0*normal (gen);
    }

    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vy += Sshear*p->x;
    }
    
    for (int k = GlobalScalarField<real>::k1;
             k < GlobalScalarField<real>::k2; ++k)
    for (int i = GlobalScalarField<real>::i1;
             i < GlobalScalarField<real>::i2; ++i)
    {
        A.x (k,i) = ampl*normal (gen);
        A.y (k,i) = ampl*normal (gen);
        A.z (k,i) = ampl*normal (gen);
    }
    boundCond (A);
    
    global->B0.x = 0.0;
    global->B0.y = 0.0;
    global->B0.z = B0;
}