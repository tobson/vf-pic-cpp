//
//  global.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "global.h"

namespace vfpic
{
static_assert (nz % nthreads == 0, "Error: 'nz' must be divisible by 'nthreads'.");
static_assert (npar % nthreads == 0, "Error: 'npar' must be divisible by 'nthreads'.");

real dx, dz;

void computeVariables ()
{
    dx = 1.0/nx;
    dz = 1.0/nz;
}
}