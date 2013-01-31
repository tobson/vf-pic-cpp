//
//  boundaries.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_boundaries_h
#define vf_pic_boundaries_h

#include "scalar-field.h"

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarBase<T,Nz,Nx>&);
template <typename T>
void boundaryConditionZ (GlobalScalarField<T>&);
template <typename T>
void boundaryCondition (GlobalScalarField<T>&);

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarBase<T,Nz,Nx>& scalar)
{
    for (int iz = 1; iz <= Nz; ++iz)
    {
        scalar(iz,0   ) = scalar(iz,Nx);
        scalar(iz,Nx+1) = scalar(iz,1 );
    }
}

template <typename T>
void boundaryConditionZ (GlobalScalarField<T>& scalar)
{
    using global::nx;
    using global::nz;
    
    for (int ix = 0; ix < nx + 2; ++ix)
    {
        scalar(0   ,ix) = scalar(nz,ix);
        scalar(nz+1,ix) = scalar(1 ,ix);
    }
}

template <typename T>
void boundaryCondition (GlobalScalarField<T>& scalar)
{
    boundaryConditionX (scalar);
    boundaryConditionZ (scalar);
}

#include "vector-field.h"

template <class S>
void boundaryConditionX (ThreeVector<S>&);
template <class S>
void boundaryConditionZ (ThreeVector<S>&);
template <class S>
void boundaryCondition (ThreeVector<S>&);

template <class S>
void boundaryConditionX (ThreeVector<S>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

template <class S>
void boundaryConditionZ (ThreeVector<S>& vector)
{
    boundaryConditionZ (vector.x);
    boundaryConditionZ (vector.y);
    boundaryConditionZ (vector.z);
}

template <class S>
void boundaryCondition (ThreeVector<S>& vector)
{
    boundaryConditionX (vector);
    boundaryConditionZ (vector);
}

#endif
