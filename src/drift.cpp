//
//  drift.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "drift.h"

#include "boundaries.h"

using namespace vfpic;

template <int Np>
void drift (Particles<Np> *particles, const real dt)
{
    Particle *p = particles->begin ();
    
    for (int dummy = 0; dummy < Np; ++dummy)
    {
        p->x = p->x + p->vx*dt;
        p->z = p->z + p->vz*dt;
        
        ++p;
    }
    
    boundaryCondition (particles);
}

template void drift (Particles<mpar>*, const real);
