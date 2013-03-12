//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "kick.h"

#include "global.h"

using namespace vfpic;

template <int Np>
void kick (const GlobalVectorField<real>& E,
           const GlobalVectorField<real>& B,
           Particles<Np> *particles, const real dt)
{
    using namespace config;
    
    const real half = 0.5;
    const real one = 1.0;
    const real two = 2.0;

    const real emdt2 = half*em*dt;

    Particle *p = particles->begin ();

#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
    for (int dummy = 0; dummy < Np; ++dummy)
    {
        const real xdx = (p->x - x0)/dx + half;
        const real zdz = (p->z - z0)/dz + half;
        
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
        
        real ex = w00*E.x (k0,i0) + w01*E.x (k0,i1) + w10*E.x (k1,i0) + w11*E.x (k1,i1);
        real ey = w00*E.y (k0,i0) + w01*E.y (k0,i1) + w10*E.y (k1,i0) + w11*E.y (k1,i1);
        real ez = w00*E.z (k0,i0) + w01*E.z (k0,i1) + w10*E.z (k1,i0) + w11*E.z (k1,i1);

        ex *= emdt2;
        ey *= emdt2;
        ez *= emdt2;
        
        real bx = w00*B.x (k0,i0) + w01*B.x (k0,i1) + w10*B.x (k1,i0) + w11*B.x (k1,i1);
        real by = w00*B.y (k0,i0) + w01*B.y (k0,i1) + w10*B.y (k1,i0) + w11*B.y (k1,i1);
        real bz = w00*B.z (k0,i0) + w01*B.z (k0,i1) + w10*B.z (k1,i0) + w11*B.z (k1,i1);
        
        bx *= emdt2;
        by *= emdt2;
        bz *= emdt2;

        if (lshear)
        {
            bz += Omega*dt;
            
            ex -= Sshear*p->x*bz;
            ez += Sshear*p->x*bx;
        }

        const real vmx = p->vx + ex;
        const real vmy = p->vy + ey;
        const real vmz = p->vz + ez;
        
        const real vpx = vmx + (vmy*bz - vmz*by);
        const real vpy = vmy + (vmz*bx - vmx*bz);
        const real vpz = vmz + (vmx*by - vmy*bx);
        
        const real fac = two/(one + bx*bx + by*by + bz*bz);
        
        p->vx = vmx + (vpy*bz - vpz*by)*fac + ex;
        p->vy = vmy + (vpz*bx - vpx*bz)*fac + ey;
        p->vz = vmz + (vpx*by - vpy*bx)*fac + ez;
        
        ++p;
    }
}

/* Explicit instantiation */
template void kick (const GlobalVectorField<real>&,
                    const GlobalVectorField<real>&,
                    Particles<mpar>*, const real);

