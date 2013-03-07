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

#include "boundaries.h"
#include "three-vector.h"
#include "variables.h"
#include "external/Faddeeva.hh"

typedef std::complex<real> complex;

complex newton (const real eps = 1e-12)
{
    using namespace config;

    const real vA2 = B0*B0/rho0;

    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;

    const real k2 = kx*kx + kz*kz;

    const real kvA = sqrt (k2*vA2);
    const real kvt = sqrt (k2)*cs0;
    const real emB = em*B0;

    const complex I = complex (0,1);
    auto Z = [] (complex zeta) { return complex (0,1)*sqrt (pi)*Faddeeva::w (zeta); };

    complex omega = kvA + kvt*kvt/(2.0*emB) - I*sqrt (0.125*pi)*(emB*emB/kvt)*exp (-0.5*emB*emB/(kvt*kvt));

    for (;;)
    {
        const complex zeta0 = sqrt (0.5)*omega/kvt;
        const complex zeta1 = sqrt (0.5)*(omega + emB)/kvt;

        const complex f = emB*(emB*zeta0*Z (zeta1) + omega) - kvA*kvA;
        const complex df = emB*(1.0 - omega*emB/(kvt*kvt) - (emB/omega)*zeta0*(2.0*zeta0*zeta1 - 1.0)*Z (zeta1));

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
    GlobalVectorField<real>& E = global->E;
    GlobalParticles& particles = global->particles;

    BoundaryConditions boundCond;
    
    const real vA2 = B0*B0/rho0;

    const real kx = 2.0*pi*real (ikx)/Lx;
    const real kz = 2.0*pi*real (ikz)/Lz;

    const real k2 = kx*kx + kz*kz;

    const real angle = atan2 (kx, kz);

    const real emB = em*B0;

    const real kvA = sqrt (k2*vA2);
    const real kvt = sqrt (k2)*cs0;

    const real eps = kvt/emB;
    const real beta = 2.0*cs0*cs0/vA2;

    std::cout << "eps = " << eps << std::endl;
    std::cout << "beta = " << beta << std::endl;

    // Imaginary unit
    const complex I (0,1);
    
    // Plasma dispersion function
    auto Z = [] (complex zeta) { return complex (0,1)*sqrt (pi)*Faddeeva::w (zeta); };

    const complex omega = newton ();

    const complex zeta0 = sqrt (0.5)*omega/kvt;
    const complex zeta1 = sqrt (0.5)*(omega + emB)/kvt;

    const real period = 2.0*pi/std::real (omega);
    std::cout << "Time steps per period: " << period/dt << std::endl;
    std::cout << "Relative damping rate: " << std::imag (omega)/std::real (omega) << std::endl;
    std::cout << "Damping per period: " << 1.0 - exp (std::imag (omega)*period) << std::endl;
    std::cout << emB*(emB*zeta0*Z (zeta1) + omega) - kvA*kvA << std::endl;

    Vector<complex> Evec, Avec, Uvec;

    Evec.x = +ampl*cos (angle);
    Evec.y = +ampl*I;
    Evec.z = -ampl*sin (angle);

//    std::cout << Evec.x << ", " << Evec.y << ", " << Evec.z << std::endl;

    for (int j = 0; j < 3; ++j) Avec[j] = Evec[j]/(I*omega);
    for (int j = 0; j < 3; ++j) Uvec[j] = em*zeta0*Z (zeta1)*Avec[j];

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
        const complex phi = kx*p->x + kz*p->z + 0.5*omega*dt;

        p->vx += std::real (Uvec.x*exp (I*phi));
        p->vy += std::real (Uvec.y*exp (I*phi));
        p->vz += std::real (Uvec.z*exp (I*phi));
    }

    for (int k = 1; k <= nz; ++k)
    for (int i = 1; i <= nx; ++i)
    {
        const complex phi = kx*grid.x (k,i) + kz*grid.z (k,i) + 0.5*omega*dt;

        for (int j = 0; j < 3; ++j) A[j] (k,i) = std::real (Avec[j]*exp (I*phi));
    }
    boundCond (A);

    for (int k = 1; k <= nz; ++k)
    for (int i = 1; i <= nx; ++i)
    {
        const complex phi = kx*grid.x (k,i) + kz*grid.z (k,i);

        for (int j = 0; j < 3; ++j) E[j] (k,i) = std::real (Evec[j]*exp (I*phi));
    }
    boundCond (E);
    
    global->B0.x = B0*sin (angle);
    global->B0.y = 0.0;
    global->B0.z = B0*cos (angle);
}