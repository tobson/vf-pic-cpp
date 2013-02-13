//
//  initial-condition.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"
#include "initial-condition.h"

#include <random>

template <typename T> T Atan2 (T, T);
template <> float Atan2 (float x, float y)
{
    return atan2f (x, y);
}
template <> double Atan2 (double x, double y)
{
    return atan2 (x, y);
}

template <typename T> T Sqrt (T);
template <> float Sqrt (float x)
{
    return sqrtf (x);
}
template <> double Sqrt (double x)
{
    return sqrt (x);
}

void initialCondition (GlobalVectorField<real>& A,
                       GlobalVectorField<real>& E,
                       GlobalParticles<real>& particles)
{
    using namespace config;
    
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
    
    const real vA2 = B0*B0/rho0;
    
    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;
    
    const real k2 = kx*kx + kz*kz;
    
    const real angle = Atan2 (kx, kz);
    
    const real Omegac = em*B0;
    
    const real kvA2 = k2*vA2;
    
    const real omega = (0.5*kvA2/Omegac)*(Sqrt(1.0 + 4.0*Omegac*Omegac/kvA2) + 1.0);
    
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
    E = real (0);
    
    boundaryCondition (A);
    boundaryCondition (E);
}