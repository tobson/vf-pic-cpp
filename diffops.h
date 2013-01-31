//
//  diffops.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/30/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_diffops_h
#define vf_pic_diffops_h

#include "scalar-field.h"
#include "three-vector.h"

#include <type_traits>

template <class S, class T>
void curl (ThreeVector<S>& B, const ThreeVector<T>& A)
{
    static_assert (is_scalarfield<S>::value, "Class S must be of ScalarField type");
    static_assert (is_scalarfield<T>::value, "Class T must be of ScalarField type");
    
    static_assert (S::nx == T::nx && S::nz == T::nz, "Field dimensions must match");

    const real fx = real (0.5)/vfpic::dx;
    const real fz = real (0.5)/vfpic::dz;

    for (int k = 1; k <= T::nz; ++k)
    for (int i = 1; i <= T::nx; ++i)
    {
        B.x (k,i) = -fz*(A.y (k+1,i  ) - A.y (k-1,i  ));
        B.z (k,i) = +fx*(A.y (k  ,i+1) - A.y (k  ,i-1));
    }
    
    for (int k = 1; k <= T::nz; ++k)
    for (int i = 1; i <= T::nx; ++i)
    {
        B.y (k,i) = fz*(A.x (k+1,i  ) - A.x (k-1,i  ))
                  - fx*(A.z (k  ,i+1) - A.z (k  ,i-1));
    }
}

template <class S, class T>
void curlcurl (ThreeVector<S>& J, const ThreeVector<T>& A)
{
    static_assert (is_scalarfield<S>::value, "Class S must be of ScalarField type");
    static_assert (is_scalarfield<T>::value, "Class T must be of ScalarField type");
    
    static_assert (S::nx == T::nx && S::nz == T::nz, "Field dimensions don't match");

    const real two = real (2.0); // wtf?
    
    const real fxx = real (1.0)/(vfpic::dx*vfpic::dx);
    const real fzz = real (1.0)/(vfpic::dz*vfpic::dz);
    
    const real fxz = real (0.25)/(vfpic::dx*vfpic::dz);
    
    for (int k = 1; k <= T::nz; ++k)
    for (int i = 1; i <= T::nx; ++i)
    {
        J.x (k,i) = fxz*(A.z (k+1,i+1) - A.z (k+1,i-1) - A.z (k-1,i+1) + A.z (k-1,i-1));
        J.z (k,i) = fxz*(A.x (k+1,i+1) - A.x (k-1,i+1) - A.x (k+1,i-1) + A.x (k-1,i-1));
        
        J.x (k,i) -= fzz*(A.x (k+1,i  ) - two*A.x (k,i) + A.x (k-1,i  ));
        J.z (k,i) -= fxx*(A.z (k  ,i+1) - two*A.z (k,i) + A.z (k  ,i-1));
    }
    
    for (int k = 1; k <= T::nz; ++k)
    for (int i = 1; i <= T::nx; ++i)
    {
        J.y (k,i) = -fxx*(A.y (k  ,i+1) - two*A.y (k,i) + A.y (k  ,i-1))
                    -fzz*(A.y (k+1,i  ) - two*A.y (k,i) + A.y (k-1,i  ));
    }
}

#endif
