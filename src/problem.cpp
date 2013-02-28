//
//  problem.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "problem.h"

#include <random>

#include "boundaries.h"
#include "initial-condition.h"

using namespace vfpic;

template <int Nz>
void boundaryConditionX (ScalarField<real,Nz,nx>& scalar)
{
    periodicBoundaryConditionX (scalar);
}
template void boundaryConditionX (LocalScalarField<real>&);
template void boundaryConditionX (std::conditional<nthreads==1,ScalarField<real,0,nx>,
                                  GlobalScalarField<real>>::type&);

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    periodicBoundaryConditionZ (scalar);
}

template <int N>
void boundaryCondition (Particles<N> *particles)
{
    periodicBoundaryCondition (particles);
}
template void boundaryCondition (LocalParticles*);
template void boundaryCondition (std::conditional<nthreads==1,Particles<0>,GlobalParticles>::type*);

void initialCondition (GlobalVariables *global)
{
    using namespace config;
    
    GlobalParticles& particles = global->particles;
    
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
    
    // Particle positions
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
    
    // Particle velocities
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vx = cs0*normal (gen);
        p->vy = cs0*normal (gen);
        p->vz = cs0*normal (gen);
    }

    // Vector potential
    global->A = real (0);
    boundaryCondition (global->A);
    
    // Electric field
    global->E = real (0);
    boundaryCondition (global->E);
    
    // Background magnetic field
    global->B0.x = 0.0;
    global->B0.y = 0.0;
    global->B0.z = 0.0;
}