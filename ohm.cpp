//
//  ohm.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ohm.h"

template <int Nz, int Nx>
void Ohm<Nz,Nx>::operator() (const VectorField<real,Nz,Nx>& B,
                             const VectorField<real,Nz,Nx>& J,
                             const ScalarField<real,Nz,Nx>& rho,
                             const VectorField<real,Nz,Nx>& ruu,
                             VectorField<real,Nz,Nx>* E)
{
    const real me = 1.0/config::em;
    
    for (int j = 0; j < 3; ++j)
    {
        
        for (int k = 1; k <= Nz; ++k)
        for (int i = 1; i <= Nx; ++i)
        {
            // Better multiply with inverse density
            U[j](k,i) = (ruu[j](k,i) - me*J[j](k,i))/rho (k,i);
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

template class Ohm<vfpic::mz,vfpic::mx>;
