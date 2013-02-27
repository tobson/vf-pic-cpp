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
#include "vector-field.h"

template <int Nz, int Nx>
void boundaryConditionX (ScalarField<real,Nz,Nx>&);
template <int Nz, int Nx>
void boundaryConditionX (VectorField<real,Nz,Nx>&);

void boundaryConditionZ (GlobalScalarField<real>&);
void boundaryConditionZ (GlobalVectorField<real>&);

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

template <int N>
void boundaryCondition (Particles<N>*);

#endif /* defined(__vf_pic__boundaries__) */
