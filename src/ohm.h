//
//  ohm.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__ohm__
#define __vf_pic__ohm__

#include "vector-field.h"

template <int Nz, int Nx>
class Ohm
{
public:
    void operator() (const VectorField<real,Nz,Nx>&,
                     const VectorField<real,Nz,Nx>&,
                     const ScalarField<real,Nz,Nx>&,
                     const VectorField<real,Nz,Nx>&,
                     VectorField<real,Nz,Nx>*);
private:
    NewVectorField<real,Nz,Nx> U;
};


#endif /* defined(__vf_pic__ohm__) */
