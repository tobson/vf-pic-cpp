//
//  faraday.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/30/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_faraday_h
#define vf_pic_faraday_h

#include "scalar-field.h"
#include "three-vector.h"

template <class S, class T>
void faraday (ThreeVector<S>& A, const ThreeVector<T>& E)
{
    static_assert (is_scalarfield<S>::value, "Class S must be of ScalarField type");
    static_assert (is_scalarfield<T>::value, "Class T must be of ScalarField type");
    
    static_assert (S::nx == T::nx && S::nz == T::nz, "Field dimensions don't match");

    const real dt = config::dt;
    
    for (int j = 0; j < 3; ++j)
    {
        for (int k = 1; k <= T::nz; ++k)
        for (int i = 1; i <= T::nx; ++i)
        {
            A[j](k,i) -= dt*E[j](k,i);
        }
    }
}

#endif
