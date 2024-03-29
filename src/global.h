//
//  global.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__global__
#define __vf_pic__global__

#include <cmath>

#include "param.h"

const real pi = real (4)*atan (real (1));

namespace vfpic
{
    const int mx = nx;
    const int mz = nz/nthreads;

    const int npar = nx*nz*npc;
    const int mpar = npar/nthreads;

    extern real dx, dz;
    extern bool lshear;
    extern real Sshear;

    void computeVariables ();
}

#endif /* defined(__vf_pic__global__) */
