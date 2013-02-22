//
//  global.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "global.h"

#include "config.h"

namespace vfpic
{
    static_assert (nz % nthreads == 0, "");
    static_assert (npar % nthreads == 0, "");

    real dx, dz;
    bool lshear;
    real Sshear;

    void computeVariables ()
    {
        using namespace config;
        
        dx = Lx/real (nx);
        dz = Lz/real (nz);

        lshear = (qshear != real (0.0));
        Sshear = -qshear*Omega;
    }
}