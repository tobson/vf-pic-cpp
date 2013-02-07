//
//  deposit.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ion-fluid.h"

Deposit::Deposit (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread),
norm (config::rho0/real (vfpic::npc))
{
}

void Deposit::operator() (const LocalParticleArrayView<real>& particles,
                          GlobalScalarField<real> *rho, GlobalVectorField<real> *ruu)
{
    using config::x0;
    using config::z0;

    using vfpic::dx;
    using vfpic::dz;
    
    const real zero = real (0.0);
    const real one = real (1.0);
    const real half = real (0.5);
    
    static_assert (std::is_pod<FourMomentum<real>>::value, "");
    const FourMomentum<real> fourzero = {zero, zero, zero, zero};
    sources.fill (fourzero);
    
    for (auto p = particles.begin (); p != particles.end (); ++p)
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
        
        sources (k0,i0).accumulate (w00,w00*p->vx,w00*p->vy,w00*p->vz);
        sources (k0,i1).accumulate (w01,w01*p->vx,w01*p->vy,w01*p->vz);
        sources (k1,i0).accumulate (w10,w10*p->vx,w10*p->vy,w10*p->vz);
        sources (k1,i1).accumulate (w11,w11*p->vx,w11*p->vy,w11*p->vz);
    }
    addGhosts ();
    convert (rho, ruu);
}

void Deposit::addGhosts ()
{
    using vfpic::nx;
    using vfpic::nz;
    
    for (int i = 0; i < nx + 2; ++i)
    {
        sources (nz,i) += sources (0   ,i);
        sources (1 ,i) += sources (nz+1,i);
    }
    for (int k = 1; k <= nz; ++k)
    {
        sources (k,nx) += sources (k,0   );
        sources (k,1 ) += sources (k,nx+1);
    }
}

void Deposit::convert (GlobalScalarField<real> *rho, GlobalVectorField<real> *ruu)
{
    {
        LocalScalarFieldView<FourMomentum<real>> sources1 (sources, ithread);

        LocalScalarFieldView<real> rho1 (*rho, ithread);
        LocalVectorFieldView<real> ruu1 (*ruu, ithread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            rho1 (k,i) = sources1 (k,i).rho;
            rux1 (k,i) = sources1 (k,i).rux;
            ruy1 (k,i) = sources1 (k,i).ruy;
            ruz1 (k,i) = sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
    
    for (int jthread = ithread + 1; jthread < ithread + vfpic::nthreads; ++jthread)
    {
        const int kthread = jthread % vfpic::nthreads;

        LocalScalarFieldView<FourMomentum<real>> sources1 (sources, kthread);

        LocalScalarFieldView<real> rho1 (*rho, kthread);
        LocalVectorFieldView<real> ruu1 (*ruu, kthread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            rho1 (k,i) += sources1 (k,i).rho;
            rux1 (k,i) += sources1 (k,i).rux;
            ruy1 (k,i) += sources1 (k,i).ruy;
            ruz1 (k,i) += sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
    // Normalize
    {
        LocalScalarFieldView<real> rho1 (*rho, ithread);
        LocalVectorFieldView<real> ruu1 (*ruu, ithread);
        
        rho1 *= norm;
        ruu1 *= norm;
    }
}
