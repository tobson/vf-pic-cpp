//
//  shearing-periodic.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "boundaries.h"
#include "particles.h"
#include "scalar-field.h"

template <int Nz>
void ProblemSpecificBoundaryConditions::X (ScalarField<real,Nz,vfpic::nx>& scalar)
{
    using namespace vfpic;
    for (int k = 1; k <= Nz; ++k)
    {
        scalar (k,0   ) = scalar (k,nx);
        scalar (k,nx+1) = scalar (k,1 );
    }
}
template void ProblemSpecificBoundaryConditions::X (GlobalScalarField<real>&);
template void ProblemSpecificBoundaryConditions::X (LocalScalarField<real>&);

void ProblemSpecificBoundaryConditions::Z (GlobalScalarField<real>& scalar)
{
    using namespace vfpic;
    for (int i = 0; i < nx+2; ++i)
    {
        scalar (0   ,i) = scalar (nz,i);
        scalar (nz+1,i) = scalar (1 ,i);
    }
}

template <int N>
void ProblemSpecificBoundaryConditions::operator() (Particles<N>& particles)
{
    using namespace config;
    using namespace vfpic;

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
template void ProblemSpecificBoundaryConditions::operator() (LocalParticles&);
