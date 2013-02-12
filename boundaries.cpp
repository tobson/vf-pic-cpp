//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

template <typename T, int Nz, int Nx>
void boundaryConditionX (ScalarField<T,Nz,Nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar(k,0   ) = scalar(k,Nx);
        scalar(k,Nx+1) = scalar(k,1 );
    }
}

template <typename T, int Nz, int Nx>
void boundaryConditionX (VectorField<T,Nz,Nx>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

template <typename T>
void boundaryConditionZ (GlobalScalarField<T>& scalar)
{
    for (int i = 0; i < nx+2; ++i)
    {
        scalar(0   ,i) = scalar(nz,i);
        scalar(nz+1,i) = scalar(1 ,i);
    }
}

template <typename T>
void boundaryConditionZ (GlobalVectorField<T>& vector)
{
    boundaryConditionZ (vector.x);
    boundaryConditionZ (vector.y);
    boundaryConditionZ (vector.z);
}

template <typename T>
void boundaryCondition (GlobalScalarField<T>& scalar)
{
    boundaryConditionX (scalar);
    boundaryConditionZ (scalar);
}

template <typename T>
void boundaryCondition (GlobalVectorField<T>& vector)
{
    boundaryConditionX (vector);
    boundaryConditionZ (vector);
}

template <typename T>
BoundaryCondition<T>::BoundaryCondition (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread)
{
}

template <typename T>
void BoundaryCondition<T>::operator() (GlobalScalarField<T>& global)
{
    LocalScalarFieldView<T> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
    barrier.wait ();
}

template <typename T>
void BoundaryCondition<T>::operator() (GlobalVectorField<T>& global)
{
    LocalVectorFieldView<T> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
    barrier.wait ();
}

template <typename T, int N>
void boundaryCondition (Particles<T,N> *particles)
{
    using config::x0;
    using config::z0;
    
    using config::Lx;
    using config::Lz;
    
    const T Lx1 = T (1)/Lx;
    const T Lz1 = T (1)/Lz;
    
    Particle<T> *p = particles->begin ();
    
    for (int dummy = 0; dummy < N; ++dummy)
    {
        p->x -= floor ((p->x - x0)*Lx1)*Lx;
        p->z -= floor ((p->z - z0)*Lz1)*Lz;
        
        ++p;
    }
}

/* Explicit instantiation */

template void boundaryConditionX (ScalarField<real,mz,mx>&);
template void boundaryConditionX (VectorField<real,mz,mx>&);
template void boundaryConditionZ (GlobalScalarField<real>&);
template void boundaryConditionZ (GlobalVectorField<real>&);
template void boundaryCondition (GlobalScalarField<real>&);
template void boundaryCondition (GlobalVectorField<real>&);

template class BoundaryCondition<real>;

template void boundaryCondition (Particles<real,mpar>*);

