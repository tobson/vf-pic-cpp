//
//  problem.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "problem.h"

#include "boundaries.h"
#include "scalar-field.h"
#include "particles.h"

using namespace vfpic;

template <int Nz>
void boundaryConditionX (ScalarField<real,Nz,nx>& scalar)
{
    periodicBoundaryConditionX (scalar);
}
template void boundaryConditionX (LocalScalarField<real>&);
template void boundaryConditionX (std::conditional<nthreads==1,ScalarField<real,0,nx>,
                                  GlobalScalarField<real>>::type&);

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    periodicBoundaryConditionZ (scalar);
}

template <int N>
void boundaryCondition (Particles<N> *particles)
{
    periodicBoundaryCondition (particles);
}
template void boundaryCondition (LocalParticles*);
template void boundaryCondition (std::conditional<nthreads==1,Particles<0>,GlobalParticles>::type*);
