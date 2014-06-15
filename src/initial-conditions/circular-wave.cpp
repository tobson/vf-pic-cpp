//
//  circular-wave.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <random>

#include "boundaries.h"
#include "variables.h"

void initialCondition (GlobalVariables *global)
{
    using namespace config;
    using namespace vfpic;

    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;

    const Grid& grid = global->grid;

    GlobalVectorField<real>& A = global->A;
    GlobalParticles& particles = global->particles;

    BoundaryConditions boundCond;

    const real vA = B0/sqrt (rho0);

    const real kx = 2.*pi*real (ikx)/Lx;

    const real oc = em*B0;

    const real kvA = kx*vA;

    const real hel = 1.;
    const real omega = kvA*sqrt (1. + .25*kvA*kvA/(oc*oc)) + .5*kvA*kvA/(hel*oc);

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
        if (int (pow (2., real (exponent))) != npc)
        {
            throw std::runtime_error ("'npc' must be a power of two");
        }
        const int npx = nx*int (pow (2., exponent/2));
        const int npz = nz*int (pow (2., exponent - exponent/2));
        for (int k = 0; k < npz; ++k)
        for (int i = 0; i < npx; ++i)
        {
            particles[k*npx + i].x = x0 + (real (i) - .5)*Lx/real (npx);
            particles[k*npx + i].z = z0 + (real (k) - .5)*Lz/real (npz);
        }
    }

    const real u_hat = -vA*ampl*kvA/omega;
    const real A_hat = B0*ampl/kx;

    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        const real phi = kx*p->x;

        p->vx = 0.;
        p->vy =  u_hat*cos (phi);
        p->vz = -u_hat*sin (phi);
    }

    for (int k = GlobalScalarField<real>::k1; k < GlobalScalarField<real>::k2; ++k)
    for (int i = GlobalScalarField<real>::i1; i < GlobalScalarField<real>::i2; ++i)
    {
        const real phi = kx*grid.x (k,i);

        A.x (k,i) = 0.;
        A.y (k,i) =  A_hat*cos (phi);
        A.z (k,i) = -A_hat*sin (phi);
    }
    boundCond (A);
    
    const real vc = dx*oc/(2.*pi);
    const real vph = vA*(sqrt(1. + .25*vA*vA/(vc*vc)) + .5*vA/vc);
    std::cout << "CFL time step: dt = " << .5*dx/vph << std::endl;

    global->B0.x = 1.;
    global->B0.y = 0.;
    global->B0.z = 0.;
}