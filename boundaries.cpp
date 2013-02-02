//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int i = 0; i < nx + 2; ++i)
    {
        scalar(0   ,i) = scalar(nz,i);
        scalar(nz+1,i) = scalar(1 ,i);
    }
}

void boundaryConditionZ (GlobalVectorField<real>& vector)
{
    boundaryConditionZ (vector.x);
    boundaryConditionZ (vector.y);
    boundaryConditionZ (vector.z);
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

void boundaryCondition (GlobalScalarField<real>& scalar)
{
    boundaryConditionX (scalar);
    boundaryConditionZ (scalar);
}

void boundaryCondition (GlobalVectorField<real>& vector)
{
    boundaryCondition (vector.x);
    boundaryCondition (vector.y);
    boundaryCondition (vector.z);
}

