//
//  cold-mri.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include <eigen3/Eigen/Eigenvalues>
#include <iomanip>
#include <iostream>
#include <random>

#include "boundaries.h"
#include "config.h"
#include "variables.h"

typedef Eigen::Matrix<std::complex<real>,4,4> Matrix;

struct EigenSolver: public Eigen::ComplexEigenSolver<Matrix>
{
    EigenSolver (const real kvA)
    {
        using namespace config;

        const real tau = 1.0/(em*B0);
        const std::complex<real> I (0,1);
        Matrix M;

        M << 0.0               , 2.0*Omega, I*kvA                     , 0.0         ,
        (qshear-2.0)*Omega, 0.0      , 0.0                       , I*kvA       ,
        I*kvA             , 0.0      , 0.0                       , -kvA*kvA*tau,
        0.0               , I*kvA    , kvA*kvA*tau - qshear*Omega, 0.0         ;

        this->compute (M);
    }
};

void initialCondition (GlobalVariables *global)
{
    using namespace config;
    using namespace vfpic;

    const real OmegaC = em*B0;
    const real vA = sqrt(B0*B0/rho0);
    std::cout << "B0 = " << B0 << ", rho0 = " << rho0 << std::endl;

    //    if (OmegaC <= 2.*Omega) throw std::runtime_error ("Cyclotron frequency must at least be twice as big as the Kepler frequency");
    const real tau = 1.0/OmegaC;
    const real kvm = Omega*sqrt (2.0*qshear/(1.0 - 2.0*Omega*tau));
    const real kvo = Omega*sqrt (((4.0 - qshear)*qshear)/(2.0*(4.0 - qshear)*Omega*tau + 4.0));
    const real kvg = (2.0*pi*vA/Lz)*round (kvo*Lz/(2.0*pi*vA));
    const real k1 = 2.0*pi/Lz;

    std::streamsize precision = std::cout.precision ();
//    std::cout << std::fixed << std::scientific << std::boolalpha;
//    std::cout << std::setprecision (std::numeric_limits<real>::digits10 + 1);
    std::cout << "Lowest non-zero wave number: kz = " << k1 << std::endl;
    std::cout << "Highest resolved wave number: kz = " << pi/dz << std::endl;
    std::cout << "Highest unstable wave number: kz = " << kvm/(k1*vA) << " k1" << std::endl;
    std::cout << "Most unstable wave number: kz = " << kvo/(k1*vA) << " k1" << std::endl;;
    std::cout << "Most unstable wave number on grid: kz = " << kvg/(k1*vA) << " k1" << std::endl;
    std::cout.precision (precision);

    const std::complex<real> I (0,1);

    {
        EigenSolver eig (kvo);
    }

    {
        EigenSolver eigenSolver (vA*pi/dz);
        Eigen::Matrix<std::complex<real>,4,1> evals = eigenSolver.eigenvalues ();

        real maxval = -1.;
        for (int n = 0; n < 4; ++n)
        {
            maxval = std::max<real> (maxval, abs (std::real (evals (n))));
            maxval = std::max<real> (maxval, abs (std::imag (evals (n))));
        }
        std::cout << "Highest frequency on the grid: " << maxval << std::endl;
        std::cout << "Corresponding time step: " << pi/maxval << std::endl;
    }

    EigenSolver eigenSolver (kvg);
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
    printf ("gamma = %.15f\n", std::real (gamma));
    std::cout << "Inverse cyclotron frequency = " << 1.0/OmegaC << std::endl;


    GlobalParticles& particles = global->particles;

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

    const real kz = kvg/vA;

    const std::complex<real> Vx = evec(0);
    const std::complex<real> Vy = evec(1);
    const std::complex<real> Bx = evec(2);
    const std::complex<real> By = evec(3);
    const std::complex<real> Ax = +By/(I*kz);
    const std::complex<real> Ay = -Bx/(I*kz);

    // Add wave
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        const std::complex<real> phase = I*kz*p->z;

        p->vx = ampl*std::real (Vx*exp (phase));
        p->vy = ampl*std::real (Vy*exp (phase));
        p->vz = 0.0;
    }

    // Add mean shear
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vy += Sshear*p->x;
    }

    BoundaryConditions boundCond;

    Grid& grid = global->grid;

    GlobalVectorField<real>& A = global->A;

    for (int k = 1; k <= nz; ++k)
    for (int i = 1; i <= nx; ++i)
    {
        const std::complex<real> phase = I*kz*grid.z (k,i);

        A.x (k,i) = ampl*std::real (Ax*exp (phase));
        A.y (k,i) = ampl*std::real (Ay*exp (phase));
        A.z (k,i) = 0.0;
    }
    boundCond (A);

    // Define operator= (real) for Vector class
    global->B0.x = 0.0;
    global->B0.y = 0.0;
    global->B0.z = B0;
}