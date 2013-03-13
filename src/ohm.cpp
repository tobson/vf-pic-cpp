//
//  ohm.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ohm.h"

template <uint Nz, uint Nx>
void computeElectronVelocity (const ScalarField<real,Nz,Nx>& J,
                              const ScalarField<real,Nz,Nx>& rho,
                              const ScalarField<real,Nz,Nx>& ruu,
                              ScalarField<real,Nz,Nx> *pU)
{
    const real me = 1.0/config::em;

    ScalarField<real,Nz,Nx>& U = *pU;
    
    for (int k = 1; k <= Nz; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (int i = 1; i <= Nx; ++i)
    {
        // Better multiply with inverse density
        U (k,i) = (ruu (k,i) - me*J (k,i))/rho (k,i);
    }
}

template <uint Nz, uint Nx>
void Ohm<Nz,Nx>::operator() (const VectorField<real,Nz,Nx>& B,
                             const VectorField<real,Nz,Nx>& J,
                             const ScalarField<real,Nz,Nx>& rho,
                             const VectorField<real,Nz,Nx>& ruu,
                             VectorField<real,Nz,Nx>* E)
{
    for (int j = 0; j < 3; ++j) computeElectronVelocity (J[j], rho, ruu[j], &U[j]);
    
    for (int k = 1; k <= Nz; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (int i = 1; i <= Nx; ++i)
    {
        E->x (k,i) = U.z (k,i)*B.y (k,i) - U.y (k,i)*B.z (k,i);
        E->y (k,i) = U.x (k,i)*B.z (k,i) - U.z (k,i)*B.x (k,i);
        E->z (k,i) = U.y (k,i)*B.x (k,i) - U.x (k,i)*B.y (k,i);
    }
}

template class Ohm<vfpic::mz,vfpic::mx>;
