//
//  cold-mri.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/21/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <eigen3/Eigen/Eigenvalues>
#include <fstream>
#include <iostream>
#include <random>

#include "boundaries.h"
#include "config.h"
#include "global.h"
#include "grid.h"
#include "particles.h"
#include "vector-field.h"
#include "three-vector.h"

int main (int argc, const char * argv[])
{
    using namespace config;
    using namespace vfpic;

    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    read (srcdir);
    vfpic::computeVariables();

    typedef Eigen::Matrix<std::complex<real>,4,4> realMatrix;
    realMatrix M;

    const real OmegaC = em*B0;
    if (OmegaC <= 2.*Omega) throw std::runtime_error ("Cyclotron frequency must at least be twice as big as the Kepler frequency");
    const real tau = 1./OmegaC;
    const real kvm = Omega*sqrt (2.*qshear/(1-2.*Omega*tau));
    const real kvo = Omega*sqrt (((4.-qshear)*qshear)/(2.*(4.-qshear)*Omega*tau + 4));
    const real kvg = (2.*pi/Lz)*round (kvo*Lz/(2.*pi));

    const real vA = sqrt(B0*B0/rho0);

    std::cout << "Highest resolved wave number: kz = " << pi/dz << std::endl;
    std::cout << "Highest unstable wave number: kz = " << kvm/vA << std::endl;
    std::cout << "Most unstable wave number: kz = " << kvo/vA << std::endl;
    std::cout << "Most unstable wave number on grid: kz = " << kvg/vA << std::endl;

    const std::complex<real> I (0,1);
    const real kvA = kvg;
    const real kz = kvA/vA;
    M << 0                , 2.*Omega, I*kvA                     , 0           ,
    (qshear-2.)*Omega, 0       , 0                         , I*kvA       ,
    I*kvA            , 0       , 0                         , -kvA*kvA*tau,
    0                , I*kvA   , kvA*kvA*tau - qshear*Omega, 0           ;

    Eigen::ComplexEigenSolver<realMatrix> eigenSolver (M);
    std::cout << eigenSolver.eigenvalues() << std::endl;
    //    std::cout << eigenSolver.eigenvectors() << std::endl;
    Eigen::Matrix<std::complex<real>,4,1> evals = eigenSolver.eigenvalues ();
    Eigen::Matrix<std::complex<real>,4,4> evecs = eigenSolver.eigenvectors ();

    real maxval = -1.;
    int nmax = 0;
    for (int n = 0; n < 4; ++n)
    {
        const real eval = std::real (evals (n));
        if (eval > maxval)
        {
            maxval = eval;
            nmax = n;
        }
    }

    const std::complex<real> gamma = evals (nmax);
    const Eigen::Matrix<std::complex<real>,4,1> evec = evecs.col (nmax);
    std::cout << "n = " << nmax << ", gamma = " << gamma << std::endl;
    std::cout << "evec = " << std::endl << evec << std::endl;

    const std::complex<real> ux = evec(0);
    const std::complex<real> uy = evec(1);
    const std::complex<real> bx = evec(2);
    const std::complex<real> by = evec(3);

    NewParticles<npar> particles;

    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;

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

    for (Particle *p = particles.begin (); p != particles.end (); ++p)
    {
        p->vx = cs0*normal (gen);
        p->vy = cs0*normal (gen);
        p->vz = cs0*normal (gen);
    }

    if (ampl > 0.0)
    {
        for (auto p = particles.begin (); p != particles.end (); ++p)
        {
            const std::complex<real> phase = I*kz*p->z - 0.5*dt*gamma;

            const std::complex<real> vx = ux*exp (phase);
            const std::complex<real> vy = uy*exp (phase);

            p->vx = ampl*std::real (vx);
            p->vy = ampl*std::real (vy);
            p->vz = 0.0;
        }
    }

    Grid grid;

    NewGlobalVectorField<real> A;
    if (ampl > 0.0)
    {
        for (int k = 1; k <= nz; ++k)
        for (int i = 1; i <= nx; ++i)
        {
            const std::complex<real> phase = I*kz*grid.z (k,i) - 0.5*dt*gamma;

            const std::complex<real> Ax = +by*exp (phase)/(I*kz);
            const std::complex<real> Ay = -bx*exp (phase)/(I*kz);
            
            A.x (k,i) = ampl*std::real (Ax);
            A.y (k,i) = ampl*std::real (Ay);
            A.z (k,i) = 0.0;
        }
    }
    boundaryCondition (A);

    Vector<real> B;
    B.x = 0.0;
    B.y = 0.0;
    B.z = B0;

    const real S = -qshear*Omega;
    NewGlobalVectorField<real> E;
    if (ampl > 0.0)
    {
        for (int k = 1; k <= nz; ++k)
        for (int i = 1; i <= nx; ++i)
        {
            const std::complex<real> phase = I*kz*grid.z (k,i);

            const std::complex<real> Ax = +by*exp (phase)/(I*kz);
            const std::complex<real> Ay = -bx*exp (phase)/(I*kz);

            const std::complex<real> Ex = -gamma*Ax - S*Ay;
            const std::complex<real> Ey = -gamma*Ay       ;

            E.x (k,i) = ampl*std::real (Ex);
            E.y (k,i) = ampl*std::real (Ey);
            E.z (k,i) = 0.0;
        }
    }
    boundaryCondition (E);

    std::ofstream fstream (srcdir + "/initial.dat", std::ios::binary);
    fstream << A << E << B << particles;
    fstream.close ();

    return 0;
}
