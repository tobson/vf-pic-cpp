//
//  ohm.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/5/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__ohm__
#define __vf_pic__ohm__

#include "ion-fluid.h"

template <typename T, int Nz, int Nx>
class Ohm
{
public:
    void operator() (const VectorBase<T,Nz,Nx>&,
                     const VectorBase<T,Nz,Nx>&,
                     const ScalarBase<T,Nz,Nx>&,
                     const VectorBase<T,Nz,Nx>&,
                     VectorBase<T,Nz,Nx>*);
private:
    NewVectorField<T,Nz,Nx> U;
};


#endif /* defined(__vf_pic__ohm__) */
