//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/8/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "faraday.h"

using namespace vfpic;

template <uint Nz, uint Nx>
void faraday (ScalarField<real,Nz,Nx>* pA, const ScalarField<real,Nz,Nx>& E, const real dt)
{
    ScalarField<real,Nz,Nx>& A = *pA;

    for (uint k = ScalarField<real,Nz,Nx>::k1;
              k < ScalarField<real,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = ScalarField<real,Nz,Nx>::i1;
              i < ScalarField<real,Nz,Nx>::i2; ++i)
    {
        A(k,i) -= dt*E(k,i);
    }
}
template <uint Nz, uint Nx>
void faraday (VectorField<real,Nz,Nx>* pA, const VectorField<real,Nz,Nx>& E, const real dt)
{
    VectorField<real,Nz,Nx>& A = *pA;

    if (lshear)
    {
        for (uint k = ScalarField<real,Nz,Nx>::k1;
                  k < ScalarField<real,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = ScalarField<real,Nz,Nx>::i1;
                  i < ScalarField<real,Nz,Nx>::i2; ++i)
        {
            A.x(k,i) -= Sshear*dt*(A.y(k,i) - 0.5*dt*E.y(k,i));
        }
    }
    
    for (uint j = 0; j < 3; ++j) faraday (&A[j], E[j], dt);
}

/* Explicit instantiation */

template void faraday (VectorField<real,mz,mx>*, const VectorField<real,mz,mx>&, const real);
