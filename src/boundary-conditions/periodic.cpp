//
//  periodic.cpp
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
    for (uint k = 1; k <= Nz; ++k)
    {
        scalar (k,0   ) = scalar (k,nx);
        scalar (k,nx+1) = scalar (k,1 );
    }
}
template void ProblemSpecificBoundaryConditions::X (GlobalScalarField<real>&);
template void ProblemSpecificBoundaryConditions::X (std::conditional<nz==mz,ScalarField<real,0,nx>,LocalScalarField<real>>::type&);

void ProblemSpecificBoundaryConditions::Z (GlobalScalarField<real>& scalar)
{
    for (uint i = 0; i < nx+2; ++i)
    {
        scalar (0   ,i) = scalar (nz,i);
        scalar (nz+1,i) = scalar (1 ,i);
    }
}

template <uint N>
void ProblemSpecificBoundaryConditions::operator() (Particles<N>& particles)
{
    using namespace config;

    const real Lx1 = 1.0/Lx;
    const real Lz1 = 1.0/Lz;

    Particle *p = particles.begin ();

    for (uint dummy = 0; dummy < N; ++dummy)
    {
        p->x -= floor ((p->x - x0)*Lx1)*Lx;
        p->z -= floor ((p->z - z0)*Lz1)*Lz;

        ++p;
    }
}
template void ProblemSpecificBoundaryConditions::operator() (GlobalParticles&);
template void ProblemSpecificBoundaryConditions::operator() (std::conditional<nz==mz,Particles<0>,LocalParticles>::type&);
