//
//  ohm.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "ohm.h"

Ohm::Ohm (const int ithread): ithread (ithread)
{
}

void Ohm::operator() (const LocalVectorField<real>& B,
                      const LocalVectorField<real>& J,
                      const LocalScalarFieldView<real>& rho,
                      const LocalVectorFieldView<real>& ruu, 
                      LocalVectorField<real>* E)
{
    const real me = real (1)/config::em;
    
    for (int j = 0; j < 3; ++j)
    {
        const LocalScalarFieldView<real>& ruj = ruu[j];
        const LocalScalarField<real>& Jj = J[j];
        
        LocalScalarField<real>& Uj = U[j];
        
        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            Uj (k,i) = (ruj (k,i) - me*Jj (k,i))/rho (k,i); // Better multiply with inverse density
        }
    }
    for (int k = 1; k <= vfpic::mz; ++k)
    for (int i = 1; i <= vfpic::mx; ++i)
    {
        E->x (k,i) = U.z (k,i)*B.y (k,i) - U.y (k,i)*B.z (k,i);
        E->y (k,i) = U.x (k,i)*B.z (k,i) - U.z (k,i)*B.x (k,i);
        E->z (k,i) = U.y (k,i)*B.x (k,i) - U.x (k,i)*B.y (k,i);
    }
}
