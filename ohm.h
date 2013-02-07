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
    Ohm (const int);
    void operator() (const VectorField<T,Nz,Nx>&,
                     const VectorField<T,Nz,Nx>&,
                     const ScalarBaseView<T,Nz,Nx>&,
                     const VectorBaseView<T,Nz,Nx>&,
                     VectorField<T,Nz,Nx>*);
private:
    const int ithread;
    VectorField<T,Nz,Nx> U;
};


#endif /* defined(__vf_pic__ohm__) */
