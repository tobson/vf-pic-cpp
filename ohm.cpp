//
//  ohm.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ohm.h"

template <typename T, int Nz, int Nx>
void Ohm<T,Nz,Nx>::operator() (const VectorField<T,Nz,Nx>& B,
                               const VectorField<T,Nz,Nx>& J,
                               const ScalarField<T,Nz,Nx>& rho,
                               const VectorField<T,Nz,Nx>& ruu,
                               VectorField<T,Nz,Nx>* E)
{
    const T me = T (1)/config::em;
    
    for (int j = 0; j < 3; ++j)
    {
        const ScalarField<T,Nz,Nx>& ruj = ruu[j];
        const ScalarField<T,Nz,Nx>& Jj = J[j];
        
        ScalarField<T,Nz,Nx>& Uj = U[j];
        
        for (int k = 1; k <= Nz; ++k)
        for (int i = 1; i <= Nx; ++i)
        {
            Uj (k,i) = (ruj (k,i) - me*Jj (k,i))/rho (k,i); // Better multiply with inverse density
        }
    }
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        E->x (k,i) = U.z (k,i)*B.y (k,i) - U.y (k,i)*B.z (k,i);
        E->y (k,i) = U.x (k,i)*B.z (k,i) - U.z (k,i)*B.x (k,i);
        E->z (k,i) = U.y (k,i)*B.x (k,i) - U.x (k,i)*B.y (k,i);
    }
}

template class Ohm<real,vfpic::mz,vfpic::mx>;
