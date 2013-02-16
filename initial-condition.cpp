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

template <typename T> T Sin (T);
template <> float Sin (float x)
{
    return sinf (x);
}
template <> double Sin (double x)
{
    return sin (x);
}

template <typename T> T Cos (T);
template <> float Cos (float x)
{
    return cosf (x);
}
template <> double Cos (double x)
{
    return cos (x);
}

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
            
            p->vx -= ampl*fac*Sin (phi)*Cos (angle);
            p->vy -= ampl*fac*Cos (phi);
            p->vz += ampl*fac*Sin (phi)*Sin (angle);
        }
    }
    
    A = real (0);
    if (ampl > 0.0)
    {
        for (int k = 1; k <= nz; ++k)
        for (int i = 1; i <= nx; ++i)
        {
            const real phi = kx*grid.x (k,i) + kz*grid.z (k,i) + 0.5*omega*dt;

            A.x (k,i) += ampl*Sin (phi)*Cos (angle);
            A.y (k,i) += ampl*Cos (phi);
            A.z (k,i) -= ampl*Sin (phi)*Sin (angle);
        }
        A /= omega;
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

            E.x (k,i) += ampl*Cos(phi)*Cos(angle);
            E.y (k,i) -= ampl*Sin(phi);
            E.z (k,i) -= ampl*Cos(phi)*Sin(angle);
        }
    }
    boundaryCondition (E);
}
