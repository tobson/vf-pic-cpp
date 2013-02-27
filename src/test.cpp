//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"
#include "global.h"
#include "particles.h"
#include "scalar-field.h"
#include "vector-field.h"

#include <iostream>
#include <vector>

int main(int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    config::read (srcdir);
    vfpic::computeVariables();
    
    if (false)
    {
        NewGlobalScalarField<float> a,b;
        std::vector<LocalScalarFieldView<float>> vec;
        vec.push_back (LocalScalarFieldView<float> (a,0));
        vec.push_back (LocalScalarFieldView<float> (b,0));
    }

//    static_assert (std::is_move_constructible<VectorField<float,vfpic::mz,vfpic::mx>>::value, "");
    static_assert (std::is_move_constructible<VectorTemplate<ScalarFieldView,float,vfpic::mz,vfpic::mx>>::value, "");
    static_assert (std::is_move_constructible<LocalVectorFieldView<float>>::value, "");
    if (false)
    {
        NewGlobalVectorField<float> a,b;
        a = b;
        NewGlobalVectorField<float> c (std::move (a));
        std::vector<LocalVectorFieldView<float>> vec;
//        vec.push_back (LocalVectorFieldView<float> (a,0));
//        vec.push_back (LocalVectorFieldView<float> (b,0));
//        LocalVectorField<real> d;
//        vec[0].x (3,7) = 3.2;
//        d = vec[1];
    }

    if (false)
    {
        NewGlobalParticles a,b;
        std::vector<LocalParticlesView> vec;
//        vec.push_back (LocalParticlesView<float> (a,0));
//        vec.push_back (LocalParticlesView<float> (b,0));
    }

    std::cout << "Hello, World! My name is " << argv[0] << ".\n";
    return 0;
}