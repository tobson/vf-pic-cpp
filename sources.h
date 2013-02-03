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

template <typename T, int N1, int N2>
struct Sources
{
    ScalarField<T,N1,N2> density;
    ThreeVector<ScalarField<T,N1,N2>> current;
};

template <typename T>
class LocalSources: public Sources<T,vfpic::mz,vfpic::mx>
{
};

template <typename T>
class GlobalSources: public Sources<T,vfpic::nz,vfpic::nx>
{
};

#endif
