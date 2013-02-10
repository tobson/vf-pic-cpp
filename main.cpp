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
#include "deposit.h"
#include "diffops.h"
#include "drift-kick.h"
#include "faraday.h"
#include "global.h"
#include "grid.h"
#include "math-helpers.h"
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
    NewGlobalVectorField<T> A, A2;
    NewGlobalParticles<T> particles, particles2;
    NewGlobalVectorField<T> E, B;
    NewGlobalScalarField<T> rho; NewGlobalVectorField<T> ruu;
};

void initialCondition (GlobalVectorField<real>& A,
                       GlobalVectorField<real>& E,
                       GlobalParticles<real>& particles)
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
    
    LocalParticlesView<real> particles  (global.particles , ithread);
    LocalParticlesView<real> particles2 (global.particles2, ithread);
    
    NewLocalVectorField<real> D;
    NewLocalVectorField<real> D2;

    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);
    
    NewLocalVectorField<real> J, H;
    
    BoundaryCondition<real> boundaryCondition (barrier, ithread);
    Deposit<real,vfpic::mpar> deposit (barrier, ithread);
    Ohm<real,vfpic::mz,vfpic::mx> ohm;
    
    for (int it = 0; it < niter; ++it)
    {
        /* Predictor step */
        
        curl (A, &B); // Better use information from previous time step
        
        faraday (A, E, dt, &A); // No need to set bc's for A if they are set for E and initial A
        
        curl (A, &H);
        
        average (B, H, &B);
        boundaryCondition (global.B);
        
        kick (global.E, global.B, particles, dt, &particles);
        
        drift (particles, 0.5*dt, &particles);
        
        deposit (particles, &global.rho, &global.ruu);
        
        drift (particles, 0.5*dt, &particles);

        curlcurl (A, &J);

        ohm (H, J, rho, ruu, &D);
        
        extrapolate (E, D, &E);
        boundaryCondition (global.E);

        /* Corrector step */

        curl (A, &B); // Better use information from predictor step
        
        faraday (A, E, dt, &A2);

        curl (A2, &H);

        average (B, H, &B);
        boundaryCondition (global.B);

        // Careful: Does this work with shear?
        kick (global.E, global.B, particles, dt, &particles2);
        
        // Positions of particles2 probably aren't right
        drift (particles2, 0.5*dt, &particles2);
        
        deposit (particles2, &global.rho, &global.ruu);

        curlcurl (A2, &J);
        
        ohm (H, J, rho, ruu, &D2);

        average (D, D2, &E);
        boundaryCondition (global.E);
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
