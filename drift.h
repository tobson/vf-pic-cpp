//
//  drift.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/11/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__drift__
#define __vf_pic__drift__

#include "particles.h"

template <typename T, int Np>
void drift (Particles<T,Np>*, const real);

#endif /* defined(__vf_pic__drift__) */
