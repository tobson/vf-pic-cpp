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

template <class S>
void faraday (ThreeVector<S>& A, const ThreeVector<S>& E)
{
    static_assert (is_scalarfield<S>::value, "Class must be of ScalarField type");

    const real dt = config::dt;
    
    for (int j = 0; j < 3; ++j)
    {
        for (int i1 = 1; i1 <= S::n1; ++i1)
        for (int i2 = 1; i2 <= S::n2; ++i2)
        {
            A[j](i1,i2) -= dt*E[j](i1,i2);
        }
    }
}

#endif
