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

template <typename T, int Np>
class Deposit
{
public:
    Deposit (Barrier&, const int);
    void operator() (const ParticleBase<T,Np>&,
                     GlobalScalarField<T>*,
                     GlobalVectorField<T>*);
private:
    void addGhosts ();
    void convert (GlobalScalarField<T>*,
                  GlobalVectorField<T>*);
    struct FourMomentum
    {
        T rho, rux, ruy, ruz;
        void accumulate (const T&, T&&, T&&, T&&);
        void operator+= (const FourMomentum&);
    };
    Barrier& barrier;
    const int ithread;
    const real norm;
    NewScalarField<FourMomentum,vfpic::nz,vfpic::nx> sources;
};

#endif /* defined(__vf_pic__deposit__) */
