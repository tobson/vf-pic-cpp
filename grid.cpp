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
    for (int k = 0; k < vfpic::mz + 2; ++k)
    for (int i = 0; i < vfpic::mx + 2; ++i)
    {
        x(k,i) = config::x0 + (real (i) - 0.5)*vfpic::dx;
        z(k,i) = config::z0 + (real (k) - 0.5)*vfpic::dz;
    }
}

std::ostream& operator<< (std::ostream& os, const Grid& grid)
{
    os << grid.x << grid.z;
    return os;
}