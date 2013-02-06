//
//  boundaries.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__boundaries__
#define __vf_pic__boundaries__

#include "barrier.h"
#include "scalar-field.h"
#include "vector-field.h"

//template <typename T, int Nz, int Nx>
//void boundaryConditionX (ScalarBase<T,Nz,Nx>&);
//template <class S>
//void boundaryConditionX (ThreeVector<S>&);

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarBase<T,Nz,Nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar(k,0   ) = scalar(k,Nx);
        scalar(k,Nx+1) = scalar(k,1 );
    }
}

template <class S>
void boundaryConditionX (ThreeVector<S>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

void boundaryConditionZ (GlobalScalarField<real>&);
void boundaryConditionZ (GlobalVectorField<real>&);

void boundaryCondition (GlobalScalarField<real>&, Barrier&, const int);
void boundaryCondition (GlobalVectorField<real>&, Barrier&, const int);

void boundaryCondition (GlobalScalarField<real>&);
void boundaryCondition (GlobalVectorField<real>&);

class BoundaryCondition
{
public:
    BoundaryCondition (Barrier&, const int);
    void operator() (GlobalScalarField<real>&);
    void operator() (GlobalVectorField<real>&);
private:
    Barrier& barrier;
    const int ithread;
};

#endif /* defined(__vf_pic__boundaries__) */
