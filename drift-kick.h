//
//  drift-kick.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__drift_kick__
#define __vf_pic__drift_kick__

#include "particles.h"
#include "vector-field.h"

template <typename T, int Np>
void drift (Particles<T,Np>*, const real);

template <typename T, int Np>
void kick (const GlobalVectorField<T>&, const GlobalVectorField<T>&,
           Particles<T,Np>*, const real);

#endif /* defined(__vf_pic__drift_kick__) */
