//
//  global-variables.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/4/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_global_variables_h
#define vf_pic_global_variables_h

template <typename T>
struct GlobalVariables
{
    GlobalVectorField<T> A, B, E, U;
    GlobalScalarField<T> rho1;
    GlobalParticleArray<T> particles;
};

#endif
