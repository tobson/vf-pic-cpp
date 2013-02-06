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

void Ohm::operator() (LocalVectorField<real>* E,
                      const LocalVectorField<real>& B,
                      const LocalVectorField<real>& J,
                      IonFluid<real>& global)
{
    const real me = real (1)/config::em;

    const LocalVectorFieldView<real> U (global.U, ithread);
    const LocalScalarFieldView<real> rho1 (global.rho1, ithread);
    
    // Compute electron fluid velocity and store it in global.U, which up to now held the ion fluid velocity.
    for (int j = 0; j < 3; ++j)
    {
        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            Ue[j] (k,i) = U[j] (k,i) - me*rho1 (k,i)*J[j] (k,i);
        }
    }
    for (int k = 1; k <= vfpic::mz; ++k)
    for (int i = 1; i <= vfpic::mx; ++i)
    {
        E->x (k,i) = Ue.z (k,i)*B.y (k,i) - Ue.y (k,i)*B.z (k,i);
        E->y (k,i) = Ue.x (k,i)*B.z (k,i) - Ue.z (k,i)*B.x (k,i);
        E->z (k,i) = Ue.y (k,i)*B.x (k,i) - Ue.x (k,i)*B.y (k,i);
    }
}