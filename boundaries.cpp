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
    static_assert (LocalScalarFieldView<real>::nx == mx, "");
    static_assert (LocalScalarFieldView<real>::nz == mz, "");
    
    for (int k = 1; k <= mz; ++k)
    {
        scalar(k,0   ) = scalar(k,nx);
        scalar(k,nx+1) = scalar(k,1 );
    }
}

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    static_assert (GlobalScalarField<real>::nx == nx, "");
    static_assert (GlobalScalarField<real>::nz == nz, "");

    for (int i = 0; i < nx + 2; ++i)
    {
        scalar(0   ,i) = scalar(nz,i);
        scalar(nz+1,i) = scalar(1 ,i);
    }
}

void boundaryConditionX (LocalVectorFieldView<real>& vector)
{
    vector.lift (boundaryConditionX);
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

void boundaryCondition (GlobalScalarField<real>& scalar)
{
    static_assert (GlobalScalarField<real>::nx == nx, "");
    static_assert (GlobalScalarField<real>::nz == nz, "");

    for (int k = 1; k <= nz; ++k)
    {
        scalar(k,0   ) = scalar(k,nx);
        scalar(k,nx+1) = scalar(k,1 );
    }
    
    for (int i = 0; i < nx + 2; ++i)
    {
        scalar(0   ,i) = scalar(nz,i);
        scalar(nz+1,i) = scalar(1 ,i);
    }
}

void boundaryCondition (GlobalVectorField<real>& vector)
{
    vector.lift (boundaryCondition);
}

