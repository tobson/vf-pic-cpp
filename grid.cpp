//
//  grid.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/29/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "config.h"
#include "grid.h"

Grid::Grid ()
{
    using namespace config;
    using namespace vfpic;
    
    for (int k = 0; k < mz + 2; ++k)
    for (int i = 0; i < mx + 2; ++i)
    {
        x(k,i) = x0 + (real (i) - 0.5)*dx;
        z(k,i) = z0 + (real (k) - 0.5)*dz;
    }
}

std::ostream& operator<< (std::ostream& os, const Grid& grid)
{
    os << grid.x;
    os << grid.z;
    return os;
}