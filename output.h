//
//  output.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/15/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__output__
#define __vf_pic__output__

#include "barrier.h"
#include "variables.h"

void output (GlobalVariables<real>&, Barrier& barrier, const long long, const int);

#endif /* defined(__vf_pic__output__) */
