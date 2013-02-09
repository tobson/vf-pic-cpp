//
//  math-helpers.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/6/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_math_helpers_h
#define vf_pic_math_helpers_h

#include "vector-field.h"

template<typename T, int Nz, int Nx>
void extrapolate (const ScalarBase<T,Nz,Nx>& start, const ScalarBase<T,Nz,Nx>& mid, ScalarBase<T,Nz,Nx>* pend)
{
    const real two = real (2);
    
    ScalarBase<T,Nz,Nx>& end = *pend;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        end (k,i) = two*mid (k,i) - start (k,i);
    }
}

template<typename T, int Nz, int Nx>
void extrapolate (const VectorBase<T,Nz,Nx>& start, const VectorBase<T,Nz,Nx>& mid, VectorBase<T,Nz,Nx>* pend)
{
    VectorBase<T,Nz,Nx>& end = *pend;
    
    for (int j = 0; j < 3; ++j) extrapolate (start[j], mid[j], &end[j]);
}

template<typename T, int Nz, int Nx>
void average (const ScalarBase<T,Nz,Nx>& start, const ScalarBase<T,Nz,Nx>& end, ScalarBase<T,Nz,Nx>* pmid)
{
    const real half = real (0.5);
    
    ScalarBase<T,Nz,Nx>& mid = *pmid;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        mid (k,i) = half*(start (k,i) + end (k,i));
    }
}

template<typename T, int Nz, int Nx>
void average (const VectorBase<T,Nz,Nx>& start, const VectorBase<T,Nz,Nx>& end, VectorBase<T,Nz,Nx>* pmid)
{
    VectorBase<T,Nz,Nx>& mid = *pmid;
    
    for (int j = 0; j < 3; ++j) average (start[j], end[j], &mid[j]);
}

#endif
