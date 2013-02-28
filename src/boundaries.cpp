//
//  boundaries.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/31/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"

using namespace vfpic;

template <int Nz>
void boundaryConditionX (VectorField<real,Nz,vfpic::nx>& vector)
{
    boundaryConditionX (vector.x);
    boundaryConditionX (vector.y);
    boundaryConditionX (vector.z);
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

template <int Nz>
void periodicBoundaryConditionX (ScalarField<real,Nz,vfpic::nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar (k,0   ) = scalar (k,nx);
        scalar (k,nx+1) = scalar (k,1 );
    }
}
void periodicBoundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int i = 0; i < nx+2; ++i)
    {
        scalar (0   ,i) = scalar (nz,i);
        scalar (nz+1,i) = scalar (1 ,i);
    }
}

template <int N>
void periodicBoundaryCondition (Particles<N> *particles)
{
    using namespace config;
    
    const real Lx1 = 1.0/Lx;
    const real Lz1 = 1.0/Lz;
    
    Particle *p = particles->begin ();
    
    if (lshear)
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
template void periodicBoundaryCondition (LocalParticles*);
template void periodicBoundaryCondition (std::conditional<nthreads==1,Particles<0>,GlobalParticles>::type*);

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

/* Explicit instantiation */

template void periodicBoundaryConditionX (LocalScalarField<real>&);
template void periodicBoundaryConditionX (std::conditional<nthreads==1,ScalarField<real,0,nx>,
                                          GlobalScalarField<real>>::type&);

template void boundaryConditionX (LocalVectorField<real>&);
template void boundaryConditionX (std::conditional<nthreads==1,VectorField<real,0,nx>,
                                  GlobalVectorField<real>>::type&);

