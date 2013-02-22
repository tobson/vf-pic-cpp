//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

template <int Nz, int Nx>
void boundaryConditionX (ScalarField<real,Nz,Nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar(k,0   ) = scalar(k,Nx);
        scalar(k,Nx+1) = scalar(k,1 );
    }
}

template <int Nz, int Nx>
void boundaryConditionX (VectorField<real,Nz,Nx>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
}

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int i = 0; i < nx+2; ++i)
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
    barrier.wait ();
}

void BoundaryCondition::operator() (GlobalVectorField<real>& global)
{
    LocalVectorFieldView<real> local (global, ithread);
    boundaryConditionX (local);
    if (barrier.wait ())
    {
        boundaryConditionZ (global);
    }
    barrier.wait ();
}

template <int N>
void boundaryCondition (Particles<N> *particles)
{
    using namespace config;
    
    const real Lx1 = 1.0/Lx;
    const real Lz1 = 1.0/Lz;
    
    Particle *p = particles->begin ();

    if (qshear != 0.0)
    {
        for (int dummy = 0; dummy < N; ++dummy)
        {
            const real i = floor ((p->x - x0)*Lx1);
            const real k = floor ((p->z - z0)*Lz1);

            p->x -= i*Lx;
            p->z -= k*Lz;

            p->vy -= i*Sshear*Lx;

            ++p;
        }
    }
    else
    {
        for (int dummy = 0; dummy < N; ++dummy)
        {
            p->x -= floor ((p->x - x0)*Lx1)*Lx;
            p->z -= floor ((p->z - z0)*Lz1)*Lz;

            ++p;
        }
    }
}

/* Explicit instantiation */

template void boundaryConditionX (ScalarField<real,mz,mx>&);
template void boundaryConditionX (VectorField<real,mz,mx>&);

template void boundaryCondition (Particles<mpar>*);

