//
//  problem.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "problem.h"

#include <random>

#include "../../src/boundaries.h"
#include "../../src/initial-condition.h"

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
    
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
    
    const Grid& grid = global->grid;
    
    GlobalVectorField<real>& A = global->A;
    GlobalVectorField<real>& E = global->E;
    GlobalParticles& particles = global->particles;
    
    const real vA2 = B0*B0/rho0;
    
    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;
    
    const real k2 = kx*kx + kz*kz;
    
    const real angle = atan2 (kx, kz);
    
    const real Omegac = em*B0;
    
    const real kvA2 = k2*vA2;
    
    const real omega = (0.5*kvA2/Omegac)*(sqrt(1.0 + 4.0*Omegac*Omegac/kvA2) + 1.0);
    
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
    
    const real fac = kvA2/(omega*omega*B0);
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        const real phi = kx*p->x + kz*p->z + 0.5*omega*dt;
        
        p->vx = -ampl*fac*sin (phi)*cos (angle);
        p->vy = -ampl*fac*cos (phi);
        p->vz = +ampl*fac*sin (phi)*sin (angle);
    }
    
    for (int k = 1; k <= nz; ++k)
    for (int i = 1; i <= nx; ++i)
    {
        const real phi = kx*grid.x (k,i) + kz*grid.z (k,i) + 0.5*omega*dt;
        
        A.x (k,i) = +(ampl/omega)*sin (phi)*cos (angle);
        A.y (k,i) = +(ampl/omega)*cos (phi);
        A.z (k,i) = -(ampl/omega)*sin (phi)*sin (angle);
    }
    boundaryCondition (A);
    
    for (int k = 1; k <= nz; ++k)
    for (int i = 1; i <= nx; ++i)
    {
        const real phi = kx*grid.x (k,i) + kz*grid.z (k,i);
        
        E.x (k,i) += ampl*cos(phi)*cos(angle);
        E.y (k,i) -= ampl*sin(phi);
        E.z (k,i) -= ampl*cos(phi)*sin(angle);
    }
    boundaryCondition (E);
    
    global->B0.x = B0*sin (angle);
    global->B0.y = 0.0;
    global->B0.z = B0*cos (angle);
}