//
//  problem.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "problem.h"

#include "scalar-field.h"
#include "particles.h"

using namespace vfpic;

void boundaryConditionZ (GlobalScalarField<real>& scalar)
{
    for (int i = 0; i < nx+2; ++i)
    {
        scalar (0   ,i) = scalar (nz,i);
        scalar (nz+1,i) = scalar (1 ,i);
    }
}

template <int Nz, int Nx>
void boundaryConditionX (ScalarField<real,Nz,Nx>& scalar)
{
    for (int k = 1; k <= Nz; ++k)
    {
        scalar (k,0   ) = scalar (k,Nx);
        scalar (k,Nx+1) = scalar (k,1 );
    }
}
template void boundaryConditionX (LocalScalarField<real>&);
template void boundaryConditionX (std::conditional<nthreads==1,ScalarField<real,nz,0>,
                                  GlobalScalarField<real>>::type&);

template <int N>
void boundaryCondition (Particles<N> *particles)
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
template void boundaryCondition (LocalParticles*);
template void boundaryCondition (std::conditional<nthreads==1,Particles<0>,GlobalParticles>::type*);
