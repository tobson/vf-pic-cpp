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

class Ohm
{
public:
    Ohm (const int);
    void operator() (LocalVectorField<real>&,
                     const LocalVectorField<real>&,
                     const LocalVectorField<real>&,
                     IonFluid<real>&);
private:
    const int ithread;
    LocalVectorField<real> Ue;
};


#endif /* defined(__vf_pic__ohm__) */
