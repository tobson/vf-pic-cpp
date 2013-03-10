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

template <int Np>
inline void Deposit<Np>::FourMomentum::accumulate
(const real& _rho, real&& _rux, real&& _ruy, real&& _ruz)
{
    rho += _rho;
    rux += _rux;
    ruy += _ruy;
    ruz += _ruz;
}

template <int Np>
inline void Deposit<Np>::FourMomentum::operator+= (const FourMomentum& other)
{
    rho += other.rho;
    rux += other.rux;
    ruy += other.ruy;
    ruz += other.ruz;
}

#endif /* defined(__vf_pic__deposit__) */
