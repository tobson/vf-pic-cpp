//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "global.h"
#include "drift-kick.h"

void drift (LocalParticleArrayView<real>& particles)
{
    using config::dt;
    
    using config::x0;
    using config::z0;
    
    using config::Lx;
    using config::Lz;
    
    const real Lx1 = real (1.0)/config::Lx;
    const real Lz1 = real (1.0)/config::Lz;
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        p->x += p->vx*dt;
        p->x -= floor ((p->x - x0)*Lx1)*Lx;

        p->z += p->vz*dt;
        p->z -= floor ((p->z - z0)*Lz1)*Lz;
    }
}

void kick (LocalParticleArrayView<real>& particles,
           const GlobalVectorField<real>& E,
           const GlobalVectorField<real>& B)
{
    using config::em;
    using config::dt;
    
    using config::x0;
    using config::z0;
    
    using vfpic::dx;
    using vfpic::dz;
    
    const real emdt2 = 0.5*em*dt;
    
    const real one = 1.0;

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
        
        const real vmx = p->vx + ex;
        const real vmy = p->vy + ey;
        const real vmz = p->vz + ez;
        
        const real vpx = vmx + (vmy*bz - vmz*by);
        const real vpy = vmy + (vmz*bx - vmx*bz);
        const real vpz = vmz + (vmx*by - vmy*bx);
        
        const real fac = 2.0/(1.0 + bx*bx + by*by + bz*bz);
        
        p->vx = vmx + (vpy*bz - vpz*by)*fac + ex;
        p->vy = vmy + (vpz*bx - vpx*bz)*fac + ey;
        p->vz = vmz + (vpx*by - vpy*bx)*fac + ez;
    }
}