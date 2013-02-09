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
#include "particles.h"
#include "scalar-field.h"
#include "vector-field.h"

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarBase<T,Nz,Nx>&);

template <typename T, int Nz, int Nx>
void boundaryConditionX (VectorBase<T,Nz,Nx>&);

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

template <typename T, int N>
void boundaryCondition (ParticleBase<T,N>&);

#endif /* defined(__vf_pic__boundaries__) */
