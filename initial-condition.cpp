//
//  initial-condition.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"
#include "initial-condition.h"

#include <cmath>
#include <random>

void initialCondition (GlobalVariables<real> *global)
{
    using namespace config;
    using namespace vfpic;
    
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;

    const Grid& grid = global->grid;
    
    GlobalVectorField<real>& A = global->A;
    GlobalVectorField<real>& E = global->E;
    GlobalParticles<real>& particles = global->particles;

    const real vA2 = B0*B0/rho0;
    
    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;
    
    const real k2 = kx*kx + kz*kz;
    
    const real angle = atan2 (kx, kz);
    
    const real Omegac = em*B0;
    
    const real kvA2 = k2*vA2;
    
    const real omega = (0.5*kvA2/Omegac)*(sqrt(1.0 + 4.0*Omegac*Omegac/kvA2) + 1.0);
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->x = x0 + uniform (gen)*Lx;
        p->z = z0 + uniform (gen)*Lz;
        
        p->vx = cs0*normal (gen);
        p->vy = cs0*normal (gen);
        p->vz = cs0*normal (gen);
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
