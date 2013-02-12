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
