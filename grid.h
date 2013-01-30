//
//  grid.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/29/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__grid__
#define __vf_pic__grid__

#include "param.h"
#include "scalar-field.h"

struct Grid
{
    GlobalScalarField<real> x, z;
    Grid ();
    friend std::ostream& operator<< (std::ostream&, const Grid&);
};

#endif /* defined(__vf_pic__grid__) */
