//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"
#include "drift-kick.h"
#include "global.h"

void drift (const LocalParticleArrayView<real>& old,
            LocalParticleArrayView<real> *pnew, const real dt)
{
    Particle<real> *lhs = pnew->begin ();
    const Particle<real> *rhs = old.begin ();
    
    for (int n = 0; n < vfpic::mpar; ++n)
    {
        lhs->x = rhs->x + rhs->vx*dt;
        lhs->z = rhs->z + rhs->vz*dt;

        ++lhs; ++rhs;
    }

    boundaryCondition (*pnew);
}

void kick (const GlobalVectorField<real>& E,
           const GlobalVectorField<real>& B,
           const LocalParticleArrayView<real>& old,
           LocalParticleArrayView<real> *pnew,
           const real dt)
{
    const real half = real (0.5);
    const real one = real (1);
    const real two = real (2);

    using config::em;
    
    using config::x0;
    using config::z0;
    
    using vfpic::dx;
    using vfpic::dz;
    
    const real emdt2 = half*em*dt;

    Particle<real> *lhs = pnew->begin ();
    const Particle<real> *rhs = old.begin ();

    for (int n = 0; n < vfpic::mpar; ++n)
    {
        const real xdx = (rhs->x - x0)/dx + half;
        const real zdz = (rhs->z - z0)/dz + half;
        
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
        
        const real vmx = rhs->vx + ex;
        const real vmy = rhs->vy + ey;
        const real vmz = rhs->vz + ez;
        
        const real vpx = vmx + (vmy*bz - vmz*by);
        const real vpy = vmy + (vmz*bx - vmx*bz);
        const real vpz = vmz + (vmx*by - vmy*bx);
        
        const real fac = two/(one + bx*bx + by*by + bz*bz);
        
        lhs->vx = vmx + (vpy*bz - vpz*by)*fac + ex;
        lhs->vy = vmy + (vpz*bx - vpx*bz)*fac + ey;
        lhs->vz = vmz + (vpx*by - vpy*bx)*fac + ez;
        
        ++lhs; ++rhs;
    }
}
