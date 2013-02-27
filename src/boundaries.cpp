//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

#include "problem.h"

using namespace vfpic;

template <int Nz, int Nx>
void boundaryConditionX (VectorField<real,Nz,Nx>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

void boundaryConditionZ (GlobalVectorField<real>& vector)
{
    boundaryConditionZ (vector.x);
    boundaryConditionZ (vector.y);
    boundaryConditionZ (vector.z);
}

void boundaryCondition (GlobalScalarField<real>& scalar)
{
    boundaryConditionX (scalar);
    boundaryConditionZ (scalar);
}

void boundaryCondition (GlobalVectorField<real>& vector)
{
    boundaryConditionX (vector);
    boundaryConditionZ (vector);
}

BoundaryCondition::BoundaryCondition (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread)
{
}

void BoundaryCondition::operator() (GlobalScalarField<real>& global)
{
    LocalScalarFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
    barrier.wait ();
}

void BoundaryCondition::operator() (GlobalVectorField<real>& global)
{
    LocalVectorFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
    barrier.wait ();
}

/* Explicit instantiation */

template void boundaryConditionX (VectorField<real,mz,mx>&);
template void boundaryConditionX (VectorField<real,nz,nx>&);
