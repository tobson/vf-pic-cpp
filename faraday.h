//
//  faraday.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/30/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_faraday_h
#define vf_pic_faraday_h

#include "vector-field.h"

void faraday (LocalVectorFieldView<real>&,
              const LocalVectorFieldView<real>&, const real);

#endif
