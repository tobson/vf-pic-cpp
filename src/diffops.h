//
//  diffops.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/30/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_diffops_h
#define vf_pic_diffops_h

#include "vector-field.h"

template <typename T, uint Nz, uint Nx>
void curl (const VectorField<T,Nz,Nx>& A, VectorField<T,Nz,Nx>* B)
{
    const real fx = real (0.5)/vfpic::dx;
    const real fz = real (0.5)/vfpic::dz;

    for (uint k = ScalarField<T,Nz,Nx>::k1;
              k < ScalarField<T,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = ScalarField<T,Nz,Nx>::i1;
              i < ScalarField<T,Nz,Nx>::i2; ++i)
    {
        B->x (k,i) = -fz*(A.y (k+1,i  ) - A.y (k-1,i  ));
        B->z (k,i) = +fx*(A.y (k  ,i+1) - A.y (k  ,i-1));
    }
    
    for (uint k = ScalarField<T,Nz,Nx>::k1;
              k < ScalarField<T,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = ScalarField<T,Nz,Nx>::i1;
              i < ScalarField<T,Nz,Nx>::i2; ++i)
    {
        B->y (k,i) = fz*(A.x (k+1,i  ) - A.x (k-1,i  ))
                   - fx*(A.z (k  ,i+1) - A.z (k  ,i-1));
    }
}

template <typename T, uint Nz, uint Nx>
void curlcurl (const VectorField<T,Nz,Nx>& A, VectorField<T,Nz,Nx>* J)
{
    const real two = real (2);
    
    const real fxx = real (1)/(vfpic::dx*vfpic::dx);
    const real fzz = real (1)/(vfpic::dz*vfpic::dz);
    
    const real fxz = real (0.25)/(vfpic::dx*vfpic::dz);
    
    for (uint k = ScalarField<T,Nz,Nx>::k1;
              k < ScalarField<T,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = ScalarField<T,Nz,Nx>::i1;
              i < ScalarField<T,Nz,Nx>::i2; ++i)
    {
        J->x (k,i) = fxz*(A.z (k+1,i+1) - A.z (k+1,i-1) - A.z (k-1,i+1) + A.z (k-1,i-1));
        J->z (k,i) = fxz*(A.x (k+1,i+1) - A.x (k-1,i+1) - A.x (k+1,i-1) + A.x (k-1,i-1));
        
        J->x (k,i) -= fzz*(A.x (k+1,i  ) - two*A.x (k,i) + A.x (k-1,i  ));
        J->z (k,i) -= fxx*(A.z (k  ,i+1) - two*A.z (k,i) + A.z (k  ,i-1));
    }
    
    for (uint k = ScalarField<T,Nz,Nx>::k1;
              k < ScalarField<T,Nz,Nx>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
    for (uint i = ScalarField<T,Nz,Nx>::i1;
              i < ScalarField<T,Nz,Nx>::i2; ++i)
    {
        J->y (k,i) = -fxx*(A.y (k  ,i+1) - two*A.y (k,i) + A.y (k  ,i-1))
                     -fzz*(A.y (k+1,i  ) - two*A.y (k,i) + A.y (k-1,i  ));
    }
}

#endif
