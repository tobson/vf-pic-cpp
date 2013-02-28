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

template <int Nz>
void boundaryConditionX (ScalarField<real,Nz,vfpic::nx>&);
template <int Nz>
void boundaryConditionX (VectorField<real,Nz,vfpic::nx>&);

void boundaryConditionZ (GlobalScalarField<real>&);
void boundaryConditionZ (GlobalVectorField<real>&);

void boundaryCondition (GlobalScalarField<real>&);
void boundaryCondition (GlobalVectorField<real>&);

template <int Nz>
void periodicBoundaryConditionX (ScalarField<real,Nz,vfpic::nx>&);
void periodicBoundaryConditionZ (GlobalScalarField<real>&);
template <int N>
void periodicBoundaryCondition (Particles<N>*);

template <int N>
void shearingPeriodicBoundaryCondition (Particles<N>*);

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
