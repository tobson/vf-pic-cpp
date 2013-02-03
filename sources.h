//
//  sources.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_sources_h
#define vf_pic_sources_h

#include "global.h"
#include "scalar-field.h"
#include "three-vector.h"

template <typename T>
struct FourCurrent
{
    T zero, one, two, three;
};

template <typename T, int N1, int N2>
using Sources = ScalarField<FourCurrent<T>,N1,N2>;

template <typename T>
using LocalSources = Sources<T,vfpic::mz,vfpic::mx>;

template <typename T>
using GlobalSources = Sources<T,vfpic::nz,vfpic::nx>;

#endif
