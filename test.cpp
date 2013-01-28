//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"
#include "global.h"
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
        global::computeVariables();
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

    {
        GlobalScalarField<float> a,b;
        std::vector<LocalScalarFieldView<float>> vec;
        vec.push_back (LocalScalarFieldView<float> (a,0));
        vec.push_back (LocalScalarFieldView<float> (b,0));
    }

    {
        GlobalVectorField<float> a,b;
        std::vector<LocalVectorFieldView<float>> vec;
        vec.push_back (LocalVectorFieldView<float> (a,0));
        vec.push_back (LocalVectorFieldView<float> (b,0));
    }

    std::cout << "Hello, World! My name is " << argv[0] << ".\n";
    return 0;
}
