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
#include "global-variables.h"
#include "particles.h"
#include "scalar-field.h"
#include "vector-field.h"

class Deposit
{
public:
    Deposit (Barrier&, const int);
    void operator() (GlobalVariables<real>&);
private:
    void addGhosts ();
    void convert (GlobalVariables<real>&);
    template <typename T>
    struct FourMomentum
    {
        T rho, rux, ruy, ruz;
        void accumulate (const T& _rho, T&& _rux, T&& _ruy, T&& _ruz)
        {
            rho += _rho;
            rux += _rux;
            ruy += _ruy;
            ruz += _ruz;
        }
        void operator+= (const FourMomentum& other)
        {
            rho += other.rho;
            rux += other.rux;
            ruy += other.ruy;
            ruz += other.ruz;
        }
    };
    Barrier& barrier;
    const int ithread;
    const real norm1;
    GlobalScalarField<FourMomentum<real>> sources;
};

#endif /* defined(__vf_pic__deposit__) */
