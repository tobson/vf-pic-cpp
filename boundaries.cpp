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
void boundaryConditionX (ScalarBase<T,Nz,Nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar(k,0   ) = scalar(k,Nx);
        scalar(k,Nx+1) = scalar(k,1 );
    }
}

template <typename T, int Nz, int Nx>
void boundaryConditionX (VectorBase<T,Nz,Nx>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

/* Explicit instantiation */
template void boundaryConditionX (VectorBase<real,vfpic::mz,vfpic::mx>&);

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int i = 0; i < nx + 2; ++i)
    {
        scalar(0   ,i) = scalar(nz,i);
        scalar(nz+1,i) = scalar(1 ,i);
    }
}

void boundaryConditionZ (GlobalVectorField<real>& vector)
{
    boundaryConditionZ (vector.x);
    boundaryConditionZ (vector.y);
    boundaryConditionZ (vector.z);
}

void boundaryCondition (GlobalScalarField<real>& global,
                        Barrier& barrier, const int ithread)
{
    LocalScalarFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
}

void boundaryCondition (GlobalVectorField<real>& global,
                        Barrier& barrier, const int ithread)
{
    LocalVectorFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
}

void boundaryCondition (GlobalScalarField<real>& scalar)
{
    boundaryConditionX (scalar);
    boundaryConditionZ (scalar);
}

void boundaryCondition (GlobalVectorField<real>& vector)
{
    boundaryConditionX (vector);
    boundaryConditionZ (vector);
}

BoundaryCondition::BoundaryCondition (Barrier& barrier, const int ithread):
barrier (barrier), ithread (ithread)
{
}

void BoundaryCondition::operator() (GlobalScalarField<real>& global)
{
    LocalScalarFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
}

void BoundaryCondition::operator() (GlobalVectorField<real>& global)
{
    LocalVectorFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
}

template <typename T, int N>
void boundaryCondition (ParticleBase<T,N>& particles)
{
    using config::x0;
    using config::z0;
    
    using config::Lx;
    using config::Lz;
    
    const T Lx1 = T (1)/Lx;
    const T Lz1 = T (1)/Lz;
    
    Particle<T> *p = particles.begin ();
    
    for (int n = 0; n < N; ++n)
    {
        p->x -= floor ((p->x - x0)*Lx1)*Lx;
        p->z -= floor ((p->z - z0)*Lz1)*Lz;
        
        ++p;
    }
}

template void boundaryCondition (ParticleBase<real,vfpic::mpar>&);