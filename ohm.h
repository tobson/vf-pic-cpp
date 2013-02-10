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

template <typename T, int Nz, int Nx>
class Ohm
{
public:
    void operator() (const VectorField<T,Nz,Nx>&,
                     const VectorField<T,Nz,Nx>&,
                     const ScalarField<T,Nz,Nx>&,
                     const VectorField<T,Nz,Nx>&,
                     VectorField<T,Nz,Nx>*);
private:
    NewVectorField<T,Nz,Nx> U;
};


#endif /* defined(__vf_pic__ohm__) */
