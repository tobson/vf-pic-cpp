//
//  drift-kick.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/8/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "faraday.h"

template<typename T, int Nz, int Nx>
void faraday (const ScalarField<T,Nz,Nx>& Aold, const ScalarField<T,Nz,Nx>& E, const real dt, ScalarField<T,Nz,Nx>* pAnew)
{
    ScalarField<T,Nz,Nx>& Anew = *pAnew;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        Anew (k,i) = Aold (k,i) - dt*E (k,i);
    }
}

template<typename T, int Nz, int Nx>
void faraday (const VectorField<T,Nz,Nx>& Aold,
              const VectorField<T,Nz,Nx>& E, const real dt, VectorField<T,Nz,Nx>* pAnew)
{
    VectorField<T,Nz,Nx>& Anew = *pAnew;
    
    for (int j = 0; j < 3; ++j) faraday (Aold[j], E[j], dt, &Anew[j]);
}

/* Explicit instantiation */

using vfpic::mx;
using vfpic::mz;

template void faraday (const VectorField<real,mz,mx>&,
                       const VectorField<real,mz,mx>&, const real,
                       VectorField<real,mz,mx>*);
