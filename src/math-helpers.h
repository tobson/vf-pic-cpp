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

template<typename T, uint Nz, uint Nx>
void extrapolate (const ScalarField<T,Nz,Nx>& start, const ScalarField<T,Nz,Nx>& mid, ScalarField<T,Nz,Nx>* pend)
{
    const real two = real (2);
    
    ScalarField<T,Nz,Nx>& end = *pend;
    
    for (uint k = 1; k <= Nz; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = 1; i <= Nx; ++i)
    {
        end (k,i) = two*mid (k,i) - start (k,i);
    }
}

template<typename T, uint Nz, uint Nx>
void extrapolate (const VectorField<T,Nz,Nx>& start, const VectorField<T,Nz,Nx>& mid, VectorField<T,Nz,Nx>* pend)
{
    VectorField<T,Nz,Nx>& end = *pend;
    
    for (uint j = 0; j < 3; ++j) extrapolate (start[j], mid[j], &end[j]);
}

template<typename T, uint Nz, uint Nx>
void average (const ScalarField<T,Nz,Nx>& start, const ScalarField<T,Nz,Nx>& end, ScalarField<T,Nz,Nx>* pmid)
{
    const real half = real (0.5);
    
    ScalarField<T,Nz,Nx>& mid = *pmid;
    
    for (uint k = 1; k <= Nz; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = 1; i <= Nx; ++i)
    {
        mid (k,i) = half*(start (k,i) + end (k,i));
    }
}

template<typename T, uint Nz, uint Nx>
void average (const VectorField<T,Nz,Nx>& start, const VectorField<T,Nz,Nx>& end, VectorField<T,Nz,Nx>* pmid)
{
    VectorField<T,Nz,Nx>& mid = *pmid;
    
    for (uint j = 0; j < 3; ++j) average (start[j], end[j], &mid[j]);
}

#endif
