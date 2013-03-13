//
//  warm-circular-wave.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 3/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <complex>
#include <iomanip>
#include <iostream>
#include <random>

#include "../boundaries.h"
#include "../misc.h"
#include "../three-vector.h"
#include "../variables.h"
#include "../external/Faddeeva.hh"

typedef std::complex<real> complex;

const complex I = complex (0,1);

inline complex Z (const complex zeta)
{
    return complex (0,1)*sqrt (pi)*Faddeeva::w (zeta);
}

template <typename T>
T sqr (const T& x)
{
    return pow (x, 2);
}

complex newton (const real eps = 1e-12)
{
    using namespace config;

    const real vA = sqrt (B0*B0/rho0);

    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;

    const real k = sqrt (kx*kx + kz*kz);

    const real kvA = k*vA;
    const real kvt = k*cs0;
    const real emB = em*B0;

    complex omega = kvA + kvt*kvt/(2.0*emB) - I*sqrt (0.125*pi)*(emB*emB/kvt)*exp (-0.5*sqr(emB/kvt));

    for (;;)
    {
        const complex zeta0 = sqrt (0.5)*omega/kvt;
        const complex zeta1 = sqrt (0.5)*(omega + emB)/kvt;
        const complex Z1 = Z (zeta1);

        const complex f = emB*(emB*zeta0*Z1 + omega) - kvA*kvA;
        const complex df = emB*(1.0 - omega*emB/(kvt*kvt) - (emB/omega)*zeta0*(2.0*zeta0*zeta1 - 1.0)*Z1);

        const complex delta = f/df;
        omega -= delta;
        if (abs (delta) < eps) break;
    }
    return omega;
}

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
    
    const real vA = sqrt (B0*B0/rho0);

    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;

    const real k = sqrt (kx*kx + kz*kz);

    const real kvA = k*vA;
    const real kvt = k*cs0;
    const real emB = em*B0;

    const real angle = atan2 (kx, kz);

    const real eps = kvt/emB;
    const real beta = 2.0*sqr (cs0/vA);

    std::cout << "eps = " << eps << std::endl;
    std::cout << "beta = " << beta << std::endl;

    const complex omega = newton ();

    const real gamma = std::imag (omega);

    const complex zeta0 = sqrt (0.5)*omega/kvt;
    const complex zeta1 = sqrt (0.5)*(omega + emB)/kvt;

    std::cout << "This should be zero: " << emB*(emB*zeta0*Z (zeta1) + omega) - kvA*kvA << std::endl;

    const real period = 2.0*pi/std::real (omega);
    std::cout << "Time steps per period: " << period/dt << std::endl;
    std::cout << "Relative damping rate: " << gamma/std::real (omega) << std::endl;
    std::cout << "Damping per period: " << 1.0 - exp (gamma*period) << std::endl;

    Vector<complex> Evec, Avec, Uvec;

    Evec.x =  ampl*cos (angle);
    Evec.y =  ampl*I;
    Evec.z = -ampl*sin (angle);

    for (uint j = 0; j < 3; ++j) Avec[j] = Evec[j]/(I*omega);
    for (uint j = 0; j < 3; ++j) Uvec[j] = Avec[j]*em*zeta0*Z (zeta1);

    // Randomize particle positions
    for (Particle *p = particles.begin (); p != particles.end (); ++p)
    {
        p->x = x0 + uniform (gen)*Lx;
        p->z = z0 + uniform (gen)*Lz;
    }

    // Thermal spread
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vx = cs0*normal (gen);
        p->vy = cs0*normal (gen);
        p->vz = cs0*normal (gen);
    }

    // Initialize wave in bulk velocity by shifting the Maxwellian
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        const real phi = kx*p->x + kz*p->z;

        p->vx += cos (phi)*std::real (Uvec.x);
        p->vy += cos (phi)*std::real (Uvec.y);
        p->vz += cos (phi)*std::real (Uvec.z);
    }
    
    removeMeanMomentum (particles);

    for (uint k = 1; k <= nz; ++k)
    for (uint i = 1; i <= nx; ++i)
    {
        const real phi = kx*grid.x (k,i) + kz*grid.z (k,i);

        for (uint j = 0; j < 3; ++j) A[j] (k,i) = cos (phi)*std::real (Avec[j]);
    }
    boundCond (A);

    global->B0.x = B0*sin (angle);
    global->B0.y = 0.0;
    global->B0.z = B0*cos (angle);
}
