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

// Problem dependend implementation
struct ProblemSpecificBoundaryConditions
{
    template <uint Nz>
    void X (ScalarField<real,Nz,vfpic::nx>&);
    void Z (GlobalScalarField<real>&);
    template <uint N>
    void operator() (Particles<N>&);
};

struct BoundaryConditions: public ProblemSpecificBoundaryConditions
{
    using ProblemSpecificBoundaryConditions::X;
    using ProblemSpecificBoundaryConditions::Z;
    using ProblemSpecificBoundaryConditions::operator();
    template <uint Nz>
    void X (VectorField<real,Nz,vfpic::nx>&);
    void Z (GlobalVectorField<real>&);
    void operator() (GlobalScalarField<real>&);
    void operator() (GlobalVectorField<real>&);
};

class BoundaryConditionsThreaded: public BoundaryConditions
{
public:
    BoundaryConditionsThreaded (Barrier&, const uint);
    void operator() (GlobalScalarField<real>&);
    void operator() (GlobalVectorField<real>&);
private:
    Barrier& barrier;
    const uint ithread;
};

#endif /* defined(__vf_pic__boundaries__) */
