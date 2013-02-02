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

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarBase<T,Nz,Nx>&);
template <class S>
void boundaryConditionX (ThreeVector<S>&);

void boundaryConditionZ (GlobalScalarField<real>&);
void boundaryConditionZ (GlobalVectorField<real>&);

void boundaryCondition (ScalarPair<real>&, Barrier&);
void boundaryCondition (VectorPair<real>&, Barrier&);

void boundaryCondition (GlobalScalarField<real>&);
void boundaryCondition (GlobalVectorField<real>&);

#endif /* defined(__vf_pic__boundaries__) */
