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
    GlobalVectorField<real> E, E2;
    GlobalVectorField<real> B;
    IonFluid<T> fluid;
    GlobalParticleArray<T> particles, particles2;
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
    
    LocalVectorFieldView<real> A (global.A, ithread);
    LocalVectorFieldView<real> E (global.E, ithread);
    
    LocalParticleArrayView<real> particles (global.particles, ithread);

    LocalVectorFieldView<real> A2 (global.A2, ithread);
    LocalVectorFieldView<real> E2 (global.E2, ithread);
    
    LocalParticleArrayView<real> particles2 (global.particles2, ithread);

    LocalVectorFieldView<real> B (global.B, ithread);
    
    IonFluid<real>& fluid = global.fluid;
    
    LocalVectorField<real> H, J, D;
    
    curl (H, A);
    
    BoundaryCondition boundaryCondition (barrier, ithread);
    Deposit deposit (barrier, ithread);
    Ohm ohm (ithread);

    for (int it = 0; it < niter; ++it)
    {
        B = H;
        
        faraday (A, E, dt);
        boundaryCondition (global.A);
        
        curl (H, A);
        
        B += H; B *= real (0.5);
        boundaryCondition (global.B);
        
        kick (particles, global.E, global.B, dt);
        drift (particles, real (0.5)*dt);
        deposit (fluid, particles);
        drift (particles, real (0.5)*dt);

        curlcurl (J, A);
        ohm (D, H, J, fluid);
        
        D *= real (2); D -= E; E2 = D;
        
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
