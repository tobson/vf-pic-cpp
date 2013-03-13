//
//  grid.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/29/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "grid.h"

#include "config.h"

Grid::Grid ()
{
    using namespace config;
    using namespace vfpic;
    
    for (uint k = 0; k < nz + 2; ++k)
    for (uint i = 0; i < nx + 2; ++i)
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