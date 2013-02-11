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
void boundaryConditionX (ScalarField<T,Nz,Nx>&);
template <typename T, int Nz, int Nx>
void boundaryConditionX (VectorField<T,Nz,Nx>&);

template <typename T>
void boundaryConditionZ (GlobalScalarField<T>&);
template <typename T>
void boundaryConditionZ (GlobalVectorField<T>&);

template <typename T>
class BoundaryCondition
{
public:
    BoundaryCondition (Barrier&, const int);
    void operator() (GlobalScalarField<T>&);
    void operator() (GlobalVectorField<T>&);
private:
    Barrier nobarrier;
    Barrier& barrier;
    const int ithread;
};

template <typename T, int N>
void boundaryCondition (Particles<T,N>&);

#endif /* defined(__vf_pic__boundaries__) */
