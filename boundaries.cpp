//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

void boundaryConditionX (LocalScalarFieldView<real>& scalar)
{
    for (int k = 1; k <= mz; ++k)
    {
        scalar(k,0   ) = scalar(k,nx);
        scalar(k,nx+1) = scalar(k,1 );
    }
}

void boundaryConditionX (LocalVectorFieldView<real>& vector)
{
    vector.lift (boundaryConditionX);
}

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int ix = 0; ix < nx + 2; ++ix)
    {
        scalar(0   ,ix) = scalar(nz,ix);
        scalar(nz+1,ix) = scalar(1 ,ix);
    }
}

void boundaryConditionZ (GlobalVectorField<real>& vector)
{
    vector.lift (boundaryConditionZ);
}

void boundaryCondition (ScalarPair<real>& scalar, Barrier& barrier)
{
    boundaryConditionX (scalar.local);
    if (barrier.wait ())
    {
        boundaryConditionZ (scalar.global);
    }
}

void boundaryCondition (VectorPair<real>& vector, Barrier& barrier)
{
    boundaryConditionX (vector.local);
    if (barrier.wait ())
    {
        boundaryConditionZ (vector.global);
    }
}

