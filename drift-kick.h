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

void drift (LocalParticleArrayView<real>&);

void kick (LocalParticleArrayView<real>&,
           const GlobalVectorField<real>&,
           const GlobalVectorField<real>&);

#endif /* defined(__vf_pic__drift_kick__) */
