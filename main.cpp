//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "boundaries.h"
#include "config.h"
#include "ion-fluid.h"
#include "diffops.h"
#include "drift-kick.h"
#include "faraday.h"
#include "global.h"
#include "grid.h"
#include "ohm.h"
#include "particles.h"
#include "vector-field.h"

#include <array>
#include <cstdio>
#include <iostream>
#include <libconfig.h++>
#include <random>
#include <thread>
#include <vector>

template <typename T>
struct GlobalVariables
{
    GlobalVectorField<real> A, A2;
    GlobalParticleArray<T> particles, particles2;
    GlobalVectorField<real> E, B;
    IonFluid<T> fluid;
};

void initialCondition (GlobalVectorField<real>& A,
                       GlobalVectorField<real>& E,
                       GlobalParticleArray<real>& particles)
{
    std::mt19937 gen;
    std::uniform_real_distribution<> uniform;
    std::normal_distribution<> normal;
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->x = config::x0 + uniform (gen)*config::Lx;
        p->z = config::z0 + uniform (gen)*config::Lz;
        
        p->vx = config::cs0*normal (gen);
        p->vy = config::cs0*normal (gen);
        p->vz = config::cs0*normal (gen);
    }
    
    A = real (0);
    E = real (0);
    
    boundaryCondition (A);
    boundaryCondition (E);
}

void iteration (GlobalVariables<real>& global, Barrier& barrier, const int ithread, int niter)
{
    using config::dt;
    
    LocalVectorFieldView<real> A  (global.A , ithread);
    LocalVectorFieldView<real> A2 (global.A2, ithread);
    
    LocalParticleArrayView<real> particles  (global.particles , ithread);
    LocalParticleArrayView<real> particles2 (global.particles2, ithread);

    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);
    
    LocalVectorField<real> D, D2;
    LocalVectorField<real> J, J2;
    LocalVectorField<real> H, H2;
    
    BoundaryCondition boundaryCondition (barrier, ithread);
    Deposit deposit (barrier, ithread);
    Ohm ohm (ithread);
    
    const real half = real (0.5);
    const real two = real (2);

    /* Store magnetic field before entering the time loop. */
    curl (&H2, A);

    for (int it = 0; it < niter; ++it)
    {
        /* Predictor step */
        
        faraday (&A, E, dt);
        boundaryCondition (global.A);
        
        curl (&H, A);
        
        for (int j = 0; j < 3; ++j)
        {
          auto Bj = B[j];
          auto Hj = H[j];
          auto H2j = H2[j];

          for (int k = 1; k <= vfpic::mz; ++k)
          for (int i = 1; i <= vfpic::mx; ++i)
          {
            Bj (k,i) = half*(Hj (k,i) + H2j (k,i));
          }
        }
        boundaryCondition (global.B);
        
        kick (&particles, global.E, global.B, dt);
        drift (&particles, half*dt);
        deposit (&global.fluid, particles);
        drift (&particles, half*dt);

        curlcurl (&J, A);
        ohm (&D, H, J, global.fluid);
        
        for (int j = 0; j < 3; ++j)
        {
          auto Dj = D[j];
          auto Ej = E[j];

          for (int k = 1; k <= vfpic::mz; ++k)
          for (int i = 1; i <= vfpic::mx; ++i)
          {
            Ej (k,i) = two*Dj (k,i) - Ej (k,i);
          }
        }

        /* Corrector step */

        faraday (&A2, A, E, dt);
        boundaryCondition (global.A2);

        curl (&H2, A2);

        for (int j = 0; j < 3; ++j)
        {
          auto Bj = B[j];
          auto Hj = H[j];
          auto H2j = H2[j];

          for (int k = 1; k <= vfpic::mz; ++k)
          for (int i = 1; i <= vfpic::mx; ++i)
          {
            Bj (k,i) = half*(Hj (k,i) + H2j (k,i));
          }
        }
        boundaryCondition (global.B);

        // Careful: Does this work with shear?
        kick (&particles2, particles, global.E, global.B, dt);
        drift (&particles2, particles, half*dt);
        deposit (&global.fluid, particles2);

        curlcurl (&J2, A2);
        ohm (&D2, H2, J2, global.fluid);

        for (int j = 0; j < 3; ++j)
        {
          auto Dj = D[j];
          auto D2j = D2[j];
          auto Ej = E[j];

          for (int k = 1; k <= vfpic::mz; ++k)
          for (int i = 1; i <= vfpic::mx; ++i)
          {
            Ej (k,i) = half*(Dj (k,i) + D2j (k,i));
          }
        }
        /* We need the right H2 in the next iteration */
        H2 = H;
    }
    printf ("Hi, I'm thread %d!\n", ithread);
}

int main (int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    try
    {
        config::read (srcdir + "/config.in");
        vfpic::computeVariables();
        config::write (srcdir + "/config.out");
    }
    catch (const libconfig::ConfigException& e)
    {
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what () << std::endl;
        return 1;
    }
    
    Barrier barrier (vfpic::nthreads);
    Grid grid;
    GlobalVariables<real> global;
    
    initialCondition (global.A, global.E, global.particles);
    
    std::vector<std::thread> threads;

    for (int ithread = 0; ithread < vfpic::nthreads; ++ithread)
    {
        std::thread thread (iteration, std::ref (global), std::ref (barrier), ithread, 16);
        threads.push_back (std::move (thread));
    }
    
    for (auto thread = begin (threads); thread != end (threads); ++thread) thread->join ();

    return 0;
}
