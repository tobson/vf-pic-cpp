//
//  deposit.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "deposit.h"

void deposit (const LocalParticleArrayView<real>& particles, LocalSources<real>& sources)
{
    using config::x0;
    using config::z0;

    using vfpic::dx;
    using vfpic::dz;
    
    const real zero = real (0.0);
    const real one = real (1.0);
    
    const FourCurrent<real> fourzero = {zero, zero, zero, zero};
    sources.fill (fourzero);
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        const real xdx = (p->x - x0)/dx + 0.5;
        const real zdz = (p->z - z0)/dz + 0.5;
        
        const int i0 (xdx);
        const int k0 (zdz);
        
        const int i1 = i0 + 1;
        const int k1 = k0 + 1;
        
        const real wx = xdx - real (i0);
        const real wz = zdz - real (k0);
        
        const real w00 = (one - wz)*(one - wx);
        const real w01 = (one - wz)*       wx ;
        const real w10 =        wz *(one - wx);
        const real w11 =        wz *       wx ;
        
        sources(k0,i0).assign (w00,w00*p->vx,w00*p->vy,w00*p->vz);
        sources(k0,i1).assign (w01,w01*p->vx,w01*p->vy,w01*p->vz);
        sources(k1,i0).assign (w10,w10*p->vx,w10*p->vy,w10*p->vz);
        sources(k1,i1).assign (w11,w11*p->vx,w11*p->vy,w11*p->vz);
    }
}
