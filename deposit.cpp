//
//  deposit.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "deposit.h"

using namespace vfpic;

template <typename T, int Np>
Deposit<T,Np>::Deposit (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread),
norm (config::rho0/T (vfpic::npc))
{
}

template <typename T, int Np>
void Deposit<T,Np>::operator() (const Particles<T,Np>& particles,
                                      GlobalScalarField<T> *rho,
                                      GlobalVectorField<T> *ruu)
{
    using namespace config;

    const T zero = T (0);
    const T one = T (1);
    const T half = T (0.5);
    
    static_assert (std::is_pod<FourMomentum>::value, "");
    const FourMomentum fourzero = {zero, zero, zero, zero};
    sources.fill (fourzero);
    
    const Particle<T> *p = particles.begin ();
    
    for (int dummy = 0; dummy <= Np; ++dummy)
    {
        const T xdx = (p->x - x0)/dx + half;
        const T zdz = (p->z - z0)/dz + half;
        
        const int i0 (xdx);
        const int k0 (zdz);
        
        const int i1 = i0 + 1;
        const int k1 = k0 + 1;
        
        const T wx = xdx - T (i0);
        const T wz = zdz - T (k0);
        
        const T w00 = (one - wz)*(one - wx);
        const T w01 = (one - wz)*       wx ;
        const T w10 =        wz *(one - wx);
        const T w11 =        wz *       wx ;
        
        sources (k0,i0).accumulate (w00,w00*p->vx,w00*p->vy,w00*p->vz);
        sources (k0,i1).accumulate (w01,w01*p->vx,w01*p->vy,w01*p->vz);
        sources (k1,i0).accumulate (w10,w10*p->vx,w10*p->vy,w10*p->vz);
        sources (k1,i1).accumulate (w11,w11*p->vx,w11*p->vy,w11*p->vz);
        
        ++p;
    }
    addGhosts ();
    convert (rho, ruu);
}

template <typename T, int Np>
void Deposit<T,Np>::addGhosts ()
{
    for (int i = 0; i < nx+2; ++i)
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

template <typename T, int Np>
void Deposit<T,Np>::convert (GlobalScalarField<T> *rho, GlobalVectorField<T> *ruu)
{
    {
        LocalScalarFieldView<FourMomentum> sources1 (sources, ithread);

        LocalScalarFieldView<real> rho1 (*rho, ithread);
        LocalVectorFieldView<real> ruu1 (*ruu, ithread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (int k = 1; k <= mz; ++k)
        for (int i = 1; i <= mx; ++i)
        {
            rho1 (k,i) = sources1 (k,i).rho;
            rux1 (k,i) = sources1 (k,i).rux;
            ruy1 (k,i) = sources1 (k,i).ruy;
            ruz1 (k,i) = sources1 (k,i).ruz;
        }
        barrier.wait ();
    }
    
    for (int jthread = ithread + 1; jthread < ithread + nthreads; ++jthread)
    {
        const int kthread = jthread % nthreads;

        LocalScalarFieldView<FourMomentum> sources1 (sources, kthread);

        LocalScalarFieldView<real> rho1 (*rho, kthread);
        LocalVectorFieldView<real> ruu1 (*ruu, kthread);

        LocalScalarFieldView<real>& rux1 = ruu1.x;
        LocalScalarFieldView<real>& ruy1 = ruu1.y;
        LocalScalarFieldView<real>& ruz1 = ruu1.z;

        for (int k = 1; k <= mz; ++k)
        for (int i = 1; i <= mx; ++i)
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

template <typename T, int Np>
void Deposit<T,Np>::FourMomentum::accumulate
(const T& _rho, T&& _rux, T&& _ruy, T&& _ruz)
{
    rho += _rho;
    rux += _rux;
    ruy += _ruy;
    ruz += _ruz;
}

template <typename T, int Np>
void Deposit<T,Np>::FourMomentum::operator+= (const FourMomentum& other)
{
    rho += other.rho;
    rux += other.rux;
    ruy += other.ruy;
    ruz += other.ruz;
}

template class Deposit<real,vfpic::mpar>;
