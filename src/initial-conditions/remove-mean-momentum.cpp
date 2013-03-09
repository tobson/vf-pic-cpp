//
//  remove-mean-momentum.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 3/8/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "remove-mean-momentum.h"

using namespace vfpic;

void removeMeanMomentum (GlobalParticles& particles)
{
    real ux = 0.0;
    real uy = 0.0;
    real uz = 0.0;
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        ux += p->vx;
        uy += p->vy;
        uz += p->vz;
    }
    
    ux /= real (npar);
    uy /= real (npar);
    uz /= real (npar);
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->vx -= ux;
        p->vy -= uy;
        p->vz -= uz;
    }
}