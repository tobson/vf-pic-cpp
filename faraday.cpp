//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/8/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "faraday.h"

template<int Nz, int Nx>
void faraday (VectorField<real,Nz,Nx>* pA, const VectorField<real,Nz,Nx>& E, const real dt)
{
    VectorField<real,Nz,Nx>& A = *pA;
    
    for (int j = 0; j < 3; ++j)
    {
        for (int k = 1; k <= Nz; ++k)
        for (int i = 1; i <= Nx; ++i)
        {
            A[j](k,i) -= dt*E[j](k,i);
        }
    }
}

/* Explicit instantiation */

using vfpic::mx;
using vfpic::mz;

template void faraday (VectorField<real,mz,mx>*, const VectorField<real,mz,mx>&, const real);
