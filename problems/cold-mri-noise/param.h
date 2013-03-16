//
//  param.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_param_h
#define vf_pic_param_h

typedef double real;
typedef unsigned int uint;

namespace vfpic
{
    const uint nx = 64;
    const uint nz = 256;

    const uint npc = 64;

    const uint nthreads = 4;

    const uint alignment = 16;

    const uint nghost = 2;
}

#endif
