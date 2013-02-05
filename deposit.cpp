//
//  deposit.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "deposit.h"

Deposit::Deposit (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread)
{
}

void Deposit::operator()(const LocalParticleArrayView<real>& particles,
                    GlobalScalarField<real>& rho, GlobalVectorField<real>& U)
{
    using config::x0;
    using config::z0;

    using vfpic::dx;
    using vfpic::dz;
    
    const real zero = real (0.0);
    const real one = real (1.0);
    
    static_assert (std::is_pod<FourMomentum<real>>::value, "");
    const FourMomentum<real> fourzero = {zero, zero, zero, zero};
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
        
        sources (k0,i0).accumulate (w00,w00*p->vx,w00*p->vy,w00*p->vz);
        sources (k0,i1).accumulate (w01,w01*p->vx,w01*p->vy,w01*p->vz);
        sources (k1,i0).accumulate (w10,w10*p->vx,w10*p->vy,w10*p->vz);
        sources (k1,i1).accumulate (w11,w11*p->vx,w11*p->vy,w11*p->vz);
    }
    addGhosts ();
    convert (rho, U);
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

void Deposit::convert (GlobalScalarField<real>& rho, GlobalVectorField<real>& U)
{
    {
        SourceView sources1 (sources, ithread);
        LocalScalarFieldView<real> rho1 (rho, ithread);
        LocalVectorFieldView<real> U1 (U, ithread);

        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            rho1 (k,i) = sources1 (k,i).rho;
            U1.x (k,i) = sources1 (k,i).rux;
            U1.y (k,i) = sources1 (k,i).ruy;
            U1.z (k,i) = sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
    
    for (int jthread = ithread + 1; jthread < ithread + vfpic::nthreads; ++jthread)
    {
        const int kthread = jthread % vfpic::nthreads;

        SourceView sources1 (sources, kthread);
        LocalScalarFieldView<real> rho1 (rho, kthread);
        LocalVectorFieldView<real> U1 (U, kthread);

        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            rho1 (k,i) += sources1 (k,i).rho;
            U1.x (k,i) += sources1 (k,i).rux;
            U1.y (k,i) += sources1 (k,i).ruy;
            U1.z (k,i) += sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
}
