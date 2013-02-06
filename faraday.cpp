//
//  faraday.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "faraday.h"

void faraday (LocalVectorFieldView<real>* A,
              const LocalVectorFieldView<real>& E, const real dt)
{
    faraday (A, *A, E, dt);
}

void faraday (LocalVectorFieldView<real>* A,
              const LocalVectorFieldView<real>& A2,
              const LocalVectorFieldView<real>& E, const real dt)
{
    for (int j = 0; j < 3; ++j)
    {
        LocalScalarFieldView<real>& Aj = (*A)[j];
        const LocalScalarFieldView<real>& A2j = A2[j];
        const LocalScalarFieldView<real>& Ej = E[j];

        for (int k = 1; k <= vfpic::mz; ++k)
        for (int i = 1; i <= vfpic::mx; ++i)
        {
            Aj(k,i) = A2j (k,i) - dt*Ej(k,i);
        }
    }
}
