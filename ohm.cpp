//
//  ohm.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ohm.h"

template <typename T, int Nz, int Nx>
void Ohm<T,Nz,Nx>::operator() (const VectorBase<T,Nz,Nx>& B,
                               const VectorBase<T,Nz,Nx>& J,
                               const ScalarBase<T,Nz,Nx>& rho,
                               const VectorBase<T,Nz,Nx>& ruu,
                               VectorBase<T,Nz,Nx>* E)
{
    const T me = T (1)/config::em;
    
    for (int j = 0; j < 3; ++j)
    {
        const ScalarBase<T,Nz,Nx>& ruj = ruu[j];
        const ScalarBase<T,Nz,Nx>& Jj = J[j];
        
        ScalarBase<T,Nz,Nx>& Uj = U[j];
        
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
