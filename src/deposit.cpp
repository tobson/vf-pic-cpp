//
//  deposit.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "deposit.h"

using namespace vfpic;

template <int Np>
Deposit<Np>::Deposit (Barrier& barrier, const uint ithread):
barrier (barrier), ithread (ithread),
norm (config::rho0/real (vfpic::npc))
{
}

template <int Np>
void Deposit<Np>::operator() (const Particles<Np>& particles,
                              GlobalScalarField<real> *rho,
                              GlobalVectorField<real> *ruu)
{
    using namespace config;

    const real zero = 0.0;
    const real one = 1.0;
    const real half = 0.5;
    
    static_assert (std::is_pod<FourMomentum>::value, "");
    const FourMomentum fourzero = {zero, zero, zero, zero};
    sources.fill (fourzero);
    
    const Particle *p = particles.begin ();
    
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
    for (int dummy = 0; dummy < Np; ++dummy)
    {
        const real xdx = (p->x - x0)/dx + half;
        const real zdz = (p->z - z0)/dz + half;
        
        const uint i0 (xdx);
        const uint k0 (zdz);
        
        const uint i1 = i0 + 1;
        const uint k1 = k0 + 1;
        
        const real wx = xdx - real (i0);
        const real wz = zdz - real (k0);
        
        const real w00 = (one - wz)*(one - wx);
        const real w01 = (one - wz)*       wx ;
        const real w10 =        wz *(one - wx);
        const real w11 =        wz *       wx ;

        const real uy = p->vy - Sshear*p->x; // Fluctuating azimuthal velocity

        sources (k0,i0).accumulate (w00,w00*p->vx,w00*uy,w00*p->vz);
        sources (k0,i1).accumulate (w01,w01*p->vx,w01*uy,w01*p->vz);
        sources (k1,i0).accumulate (w10,w10*p->vx,w10*uy,w10*p->vz);
        sources (k1,i1).accumulate (w11,w11*p->vx,w11*uy,w11*p->vz);
        
        ++p;
    }
    // Add ghost zones
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
    for (uint i = 0; i < nx+2; ++i)
    {
        sources (nz,i) += sources (0   ,i);
        sources (1 ,i) += sources (nz+1,i);
    }
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
    for (uint k = 1; k <= nz; ++k)
    {
        sources (k,nx) += sources (k,0   );
        sources (k,1 ) += sources (k,nx+1);
    }
    convert (rho, ruu);
}

template <int Np>
void Deposit<Np>::convert (GlobalScalarField<real> *rho, GlobalVectorField<real> *ruu)
{
    {
        LocalScalarFieldView<FourMomentum> sources1 (sources, ithread);

        LocalScalarFieldView<real> rho1 (*rho, ithread);
        LocalVectorFieldView<real> ruu1 (*ruu, ithread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (uint k = 1; k <= mz; ++k)
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
        for (uint i = 1; i <= mx; ++i)
        {
            rho1 (k,i) = sources1 (k,i).rho;
            rux1 (k,i) = sources1 (k,i).rux;
            ruy1 (k,i) = sources1 (k,i).ruy;
            ruz1 (k,i) = sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
    
    for (uint jthread = ithread + 1; jthread < ithread + nthreads; ++jthread)
    {
        const uint kthread = jthread % nthreads;

        LocalScalarFieldView<FourMomentum> sources1 (sources, kthread);

        LocalScalarFieldView<real> rho1 (*rho, kthread);
        LocalVectorFieldView<real> ruu1 (*ruu, kthread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (uint k = 1; k <= mz; ++k)
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
        for (uint i = 1; i <= mx; ++i)
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

template class Deposit<vfpic::mpar>;
