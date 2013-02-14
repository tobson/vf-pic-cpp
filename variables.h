//
//  variables.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/13/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_variables_h
#define vf_pic_variables_h

#include "particles.h"
#include "scalar-field.h"
#include "vector-field.h"

template <typename T>
struct GlobalVariables
{
    NewGlobalVectorField<T> A, A2;
    NewGlobalParticles<T> particles, particles2;
    NewGlobalVectorField<T> E, B;
    NewGlobalScalarField<T> rho; NewGlobalVectorField<T> ruu;
    std::array<T,3> B0;
};

#endif
