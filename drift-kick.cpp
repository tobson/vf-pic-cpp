//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "global.h"
#include "drift-kick.h"

void drift (LocalParticleArrayView<real>* particles, const real dt)
{
    drift (particles, *particles, dt);
}

void drift (LocalParticleArrayView<real>* particles,
            const LocalParticleArrayView<real>& particles2, const real dt)
{
    using config::x0;
    using config::z0;
    
    using config::Lx;
    using config::Lz;
    
    const real Lx1 = real (1.0)/config::Lx;
    const real Lz1 = real (1.0)/config::Lz;
    
    auto pnew = particles->begin ();
    auto pold = particles2.begin ();
    
    for (int n = 0; n < vfpic::mpar; ++n)
    {
        pnew->x = pold->x + pnew->vx*dt;
        pnew->x -= floor ((pnew->x - x0)*Lx1)*Lx;

        pnew->z = pold->z + pnew->vz*dt;
        pnew->z -= floor ((pnew->z - z0)*Lz1)*Lz;
        
        ++pnew;
        ++pold;
    }
}

void kick (LocalParticleArrayView<real>* particles,
           const GlobalVectorField<real>& E,
           const GlobalVectorField<real>& B, const real dt)
{
    kick (particles, *particles, E, B, dt);
}

void kick (LocalParticleArrayView<real>* particles,
           const LocalParticleArrayView<real>& particles2,
           const GlobalVectorField<real>& E,
           const GlobalVectorField<real>& B, const real dt)
{
    using config::em;
    
    using config::x0;
    using config::z0;
    
    using vfpic::dx;
    using vfpic::dz;
    
    const real emdt2 = 0.5*em*dt;
    
    const real one = real (1.0);
    const real two = real (2.0);

    auto pnew = particles->begin ();
    auto pold = particles2.begin ();

    for (int n = 0; n < vfpic::mpar; ++n)
    {
        const real xdx = (pnew->x - x0)/dx + 0.5;
        const real zdz = (pnew->z - z0)/dz + 0.5;
        
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
        
        const real vmx = pold->vx + ex;
        const real vmy = pold->vy + ey;
        const real vmz = pold->vz + ez;
        
        const real vpx = vmx + (vmy*bz - vmz*by);
        const real vpy = vmy + (vmz*bx - vmx*bz);
        const real vpz = vmz + (vmx*by - vmy*bx);
        
        const real fac = two/(one + bx*bx + by*by + bz*bz);
        
        pnew->vx = vmx + (vpy*bz - vpz*by)*fac + ex;
        pnew->vy = vmy + (vpz*bx - vpx*bz)*fac + ey;
        pnew->vz = vmz + (vpx*by - vpy*bx)*fac + ez;
        
        ++pnew;
        ++pold;
    }
}