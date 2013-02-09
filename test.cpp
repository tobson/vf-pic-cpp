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
#include <libconfig.h++>
#include <vector>

int main(int argc, const char * argv[])
{
    std::string srcdir = ".";
    if (argc > 1) srcdir = argv[1];

    try
    {
        config::read (srcdir + "/config.in");
        vfpic::computeVariables();
        config::write (srcdir + "/config.out");
    }
    catch (const libconfig::ConfigException& e)
    {
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what () << std::endl;
        return 1;
    }

    if (false)
    {
        NewGlobalScalarField<float> a,b;
        std::vector<LocalScalarFieldView<float>> vec;
        vec.push_back (LocalScalarFieldView<float> (a,0));
        vec.push_back (LocalScalarFieldView<float> (b,0));
    }

    {
        NewGlobalVectorField<float> a,b;
        a = b;
        GlobalVectorField<float> c (std::move (a));
        std::vector<LocalVectorFieldView<float>> vec;
        vec.push_back (LocalVectorFieldView<float> (a,0));
//        vec.push_back (LocalVectorFieldView<float> (b,0));
//        LocalVectorField<real> d;
//        vec[0].x (3,7) = 3.2;
//        d = vec[1];
    }

    if (false)
    {
        GlobalParticleArray<float> a,b;
        std::vector<LocalParticleArrayView<float>> vec;
        vec.push_back (LocalParticleArrayView<float> (a,0));
        vec.push_back (LocalParticleArrayView<float> (b,0));
    }

    std::cout << "Hello, World! My name is " << argv[0] << ".\n";
    return 0;
}
