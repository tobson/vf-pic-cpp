//
//  drift.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"
#include "drift.h"

using namespace vfpic;

template <typename T, int Np>
void drift (Particles<T,Np> *particles, const real dt)
{
    Particle<T> *p = particles->begin ();
    
    for (int dummy = 0; dummy < Np; ++dummy)
    {
        p->x = p->x + p->vx*dt;
        p->z = p->z + p->vz*dt;
        
        ++p;
    }
    
    boundaryCondition (*particles);
}

template void drift (Particles<real,mpar>*, const real);