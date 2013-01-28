//
//  global.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__global__
#define __vf_pic__global__

#include "param.h"

namespace global
{
  const int mx = nx;
  const int mz = nz/nthreads;
  
  const int npar = nx*nz*npc;
  const int mpar = npar/nthreads;
  
  extern real dx, dz;
  
  void computeVariables ();
}

#endif /* defined(__vf_pic__global__) */
