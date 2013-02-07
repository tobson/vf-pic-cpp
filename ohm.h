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
    void operator() (const LocalVectorField<real>&,
                     const LocalVectorField<real>&,
                     const LocalScalarFieldView<real>&,
                     const LocalVectorFieldView<real>&,
                     LocalVectorField<real>*);
private:
    const int ithread;
    LocalVectorField<real> U;
};


#endif /* defined(__vf_pic__ohm__) */
