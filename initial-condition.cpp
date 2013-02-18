//
//  initial-condition.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "initial-condition.h"

#include <cmath>
#include <random>

#include "boundaries.h"

void initialCondition (GlobalVariables *global)
{
    using namespace config;
    using namespace vfpic;
    
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
    
    for (int n = 0; n < npar; ++n)
    {
        // Use angle here so that 1D in x is exactly the same as 1D in z
        // Also try uniform grid in particle space
//        p->x = x0 + uniform (gen)*Lx;
//        p->z = z0 + uniform (gen)*Lz;

//        particles[n].x = x0 + (real (n) - 0.5)*Lx/real (npar);
//        particles[n].z = 0.0;

        particles[n].x = 0.0;
        particles[n].z = z0 + (real (n) - 0.5)*Lz/real (npar);

        particles[n].vx = cs0*normal (gen);
        particles[n].vy = cs0*normal (gen);
        particles[n].vz = cs0*normal (gen);
    }
    
    if (ampl > 0.0)
    {
        const real fac = kvA2/(omega*omega*B0);
        
        for (auto p = particles.begin (); p != particles.end (); ++p)
        {
            const real phi = kx*p->x + kz*p->z + 0.5*omega*dt;
            
            p->vx -= ampl*fac*sin (phi)*cos (angle);
            p->vy -= ampl*fac*cos (phi);
            p->vz += ampl*fac*sin (phi)*sin (angle);
        }
    }
    
    A = real (0);
    if (ampl > 0.0)
    {
        const real fac = ampl/omega;
        
        for (int k = 1; k <= nz; ++k)
        for (int i = 1; i <= nx; ++i)
        {
            const real phi = kx*grid.x (k,i) + kz*grid.z (k,i) + 0.5*omega*dt;

            A.x (k,i) += fac*sin (phi)*cos (angle);
            A.y (k,i) += fac*cos (phi);
            A.z (k,i) -= fac*sin (phi)*sin (angle);
        }
    }
    boundaryCondition (A);

    global->B0.x = B0*sin (angle);
    global->B0.y = 0.0;
    global->B0.z = B0*cos (angle);

    E = real (0);
    if (ampl > 0.0)
    {
        for (int k = 1; k <= nz; ++k)
        for (int i = 1; i <= nx; ++i)
        {
            const real phi = kx*grid.x (k,i) + kz*grid.z (k,i);

            E.x (k,i) += ampl*cos(phi)*cos(angle);
            E.y (k,i) -= ampl*sin(phi);
            E.z (k,i) -= ampl*cos(phi)*sin(angle);
        }
    }
    boundaryCondition (E);
}
