//
//  shearing-periodic.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "../boundaries.h"
#include "../particles.h"
#include "../scalar-field.h"

using namespace vfpic;

template <uint Nz>
void ProblemSpecificBoundaryConditions::X (ScalarField<real,Nz,nx>& scalar)
{
    const int i1 = ScalarField<real,Nz,nx>::i1;
    const int i2 = ScalarField<real,Nz,nx>::i2;
    const int k1 = ScalarField<real,Nz,nx>::k1;
    const int k2 = ScalarField<real,Nz,nx>::k2;
    for (uint k = k1; k < k2; ++k)
    {
        scalar (k,i1-1) = scalar (k,i2-1);
        scalar (k,i2  ) = scalar (k,i1  );
    }
}
template void ProblemSpecificBoundaryConditions::X (GlobalScalarField<real>&);
template void ProblemSpecificBoundaryConditions::X (std::conditional<nz==mz,ScalarField<real,0,nx>,LocalScalarField<real>>::type&);

void ProblemSpecificBoundaryConditions::Z (GlobalScalarField<real>& scalar)
{
    const int i1 = GlobalScalarField<real>::i1;
    const int i2 = GlobalScalarField<real>::i2;
    const int k1 = GlobalScalarField<real>::k1;
    const int k2 = GlobalScalarField<real>::k2;
    for (uint i = i1-1; i < i2+1; ++i)
    {
        scalar (k1-1,i) = scalar (k2-1,i);
        scalar (k2  ,i) = scalar (k1  ,i);
    }
}

template <uint N>
void ProblemSpecificBoundaryConditions::operator() (Particles<N>& particles)
{
    using namespace config;

    const real Lx1 = 1.0/Lx;
    const real Lz1 = 1.0/Lz;

    Particle *p = particles.begin ();

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
template void ProblemSpecificBoundaryConditions::operator() (GlobalParticles&);
template void ProblemSpecificBoundaryConditions::operator() (std::conditional<nz==mz,Particles<0>,LocalParticles>::type&);
