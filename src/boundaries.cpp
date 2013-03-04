//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

// Implementation of class BoundaryConditions
template <int Nz>
void BoundaryConditions::X (VectorField<real,Nz,nx>& vector)
{
    X (vector.x);
    X (vector.y);
    X (vector.z);
}
template void BoundaryConditions::X (GlobalVectorField<real>&);
template void BoundaryConditions::X (std::conditional<mz==nz,VectorField<real,0,nx>,LocalVectorField<real>>::type&);

void BoundaryConditions::Z (GlobalVectorField<real>& vector)
{
    Z (vector.x);
    Z (vector.y);
    Z (vector.z);
}

void BoundaryConditions::operator() (GlobalScalarField<real>& scalar)
{
    X (scalar);
    Z (scalar);
}

void BoundaryConditions::operator() (GlobalVectorField<real>& vector)
{
    this->operator() (vector.x);
    this->operator() (vector.y);
    this->operator() (vector.z);
}

// Implementation of class BoundaryConditionsThreaded
BoundaryConditionsThreaded::BoundaryConditionsThreaded
(Barrier& barrier, const int ithread): barrier (barrier), ithread (ithread) {}

void BoundaryConditionsThreaded::operator() (GlobalScalarField<real>& global)
{
    LocalScalarFieldView<real> local (global, ithread);
    X (local);
    if (barrier.wait ()) Z (global);
    barrier.wait ();
}

void BoundaryConditionsThreaded::operator() (GlobalVectorField<real>& global)
{
    LocalVectorFieldView<real> local (global, ithread);
    X (local);
    if (barrier.wait ()) Z (global);
    barrier.wait ();
}
