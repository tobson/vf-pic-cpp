//
//  deposit.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__deposit__
#define __vf_pic__deposit__

#include "barrier.h"
#include "particles.h"
#include "scalar-field.h"
#include "vector-field.h"

template <int Np>
class Deposit
{
public:
    Deposit (Barrier&, const int);
    void operator() (const Particles<Np>&,
                     GlobalScalarField<real>*,
                     GlobalVectorField<real>*);
private:
    void convert (GlobalScalarField<real>*,
                  GlobalVectorField<real>*);
    struct FourMomentum
    {
        real rho, rux, ruy, ruz;
        void accumulate (const real&, real&&, real&&, real&&);
        void operator+= (const FourMomentum&);
    };
    Barrier& barrier;
    const int ithread;
    const real norm;
    NewGlobalScalarField<FourMomentum> sources;
};

#endif /* defined(__vf_pic__deposit__) */
