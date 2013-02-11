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
    
    BoundaryCondition<real> boundaryCondition;
    
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
    
    NewLocalVectorField<real> D, D2;
    NewLocalVectorField<real> H, H2;
    NewLocalVectorField<real> J;

    LocalVectorFieldView<real> E (global.E, ithread);
    LocalVectorFieldView<real> B (global.B, ithread);

    LocalScalarFieldView<real> rho (global.rho, ithread);
    LocalVectorFieldView<real> ruu (global.ruu, ithread);
    
    BoundaryCondition<real> boundaryCondition (barrier, ithread);
    Deposit<real,vfpic::mpar> deposit (barrier, ithread);
    Ohm<real,vfpic::mz,vfpic::mx> ohm;
    
    curl (A, &H2);
    
    for (int it = 0; it < niter; ++it)
    {
        /* Predictor step */
        
        faraday (&A, E, dt);
        boundaryCondition (global.A);
        
        curl (A, &H);
        curlcurl (A, &J);
                
        average (H2, H, &B);

        /* It would be nice if boundaryCondition::operator()
           would be a variadic function */
        boundaryCondition (global.E);
        boundaryCondition (global.B);
        
        kick (global.E, global.B, &particles, dt);
        
        drift (&particles, 0.5*dt);
        
        deposit (particles, &global.rho, &global.ruu);
        
        drift (&particles, 0.5*dt);

        ohm (H, J, rho, ruu, &D);
        
        extrapolate (E, D, &E);
        
        /* Save for next time step */
        H2 = H;
        
        /* Make copy of dynamice variables */
        A2 = A;
        particles2 = particles;

        /* Corrector step */

        faraday (&A2, E, dt);
        boundaryCondition (global.A2);

        curl (A2, &H);
        curlcurl (A2, &J);

        average (H2, H, &B);

        boundaryCondition (global.E);
        boundaryCondition (global.B);

        kick (global.E, global.B, &particles2, dt);
        
        drift (&particles2, 0.5*dt);
        
        deposit (particles2, &global.rho, &global.ruu);
        
        ohm (H, J, rho, ruu, &D2);

        average (D, D2, &E);
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
