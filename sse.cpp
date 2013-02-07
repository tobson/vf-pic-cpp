//
//  sse.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/1/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "math-helpers.h"
#include "vector-field.h"

#include <iostream>

int main (int argc, const char * argv[])
{
    GlobalVectorField<float> A, B, C;
    A = float (1);
    B = float (2);
    extrapolate (A, B, &C);
    std::cout << A.y (3, 6) << std::endl;
    std::cout << B.y (3, 6) << std::endl;
    std::cout << C.y (3, 6) << std::endl;
    average (A, &B, C);
    return 0;
}