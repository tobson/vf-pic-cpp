//
//  math-helpers.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/6/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_math_helpers_h
#define vf_pic_math_helpers_h

#include "three-vector.h"

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void extrapolate (const S1<T,Nz,Nx>& start, const S2<T,Nz,Nx>& mid, S3<T,Nz,Nx>* pend)
{
    const real two = real (2);
    
    S3<T,Nz,Nx>& end = *pend;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        end (k,i) = two*mid (k,i) - start (k,i);
    }
}

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void extrapolate (const ThreeVector<S1,T,Nz,Nx>& start, const ThreeVector<S2,T,Nz,Nx>& mid, ThreeVector<S3,T,Nz,Nx>* pend)
{
    ThreeVector<S3,T,Nz,Nx>& end = *pend;
    
    for (int j = 0; j < 3; ++j) extrapolate (start[j], mid[j], &end[j]);
}

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void average (const S1<T,Nz,Nx>& start, const S3<T,Nz,Nx>& end, S2<T,Nz,Nx>* pmid)
{
    const real half = real (0.5);
    
    S2<T,Nz,Nx>& mid = *pmid;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        mid (k,i) = half*(start (k,i) + end (k,i));
    }
}

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void average (const ThreeVector<S1,T,Nz,Nx>& start, const ThreeVector<S3,T,Nz,Nx>& end, ThreeVector<S2,T,Nz,Nx>* pmid)
{
    ThreeVector<S2,T,Nz,Nx>& mid = *pmid;
    
    for (int j = 0; j < 3; ++j) average (start[j], end[j], &mid[j]);
}

#endif
