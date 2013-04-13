//
//  variables.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/13/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_variables_h
#define vf_pic_variables_h

#include <fstream>

#include "grid.h"
#include "particles.h"
#include "scalar-field.h"
#include "three-vector.h"
#include "vector-field.h"

struct GlobalVariables
{
    NewGlobalVectorField<real> A, A2;
    NewGlobalParticles particles, particles2;
    NewGlobalVectorField<real> E, E2;
    NewGlobalVectorField<real> B, B2;
    NewGlobalScalarField<real> rho; NewGlobalVectorField<real> ruu;
    Vector<real> B0;
    Grid grid;
    std::ofstream datafile;
};

#endif
