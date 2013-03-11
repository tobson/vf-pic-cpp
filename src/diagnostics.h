//
//  diagnostics.h
//  vf-pic
//
//  Created by Tobias Heinemann on 3/10/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__diagnostics__
#define __vf_pic__diagnostics__

#include <array>
#include <map>

#include "barrier.h"
#include "param.h"
#include "variables.h"
#include "vector-field.h"

struct Diagnostics
{
//    std::map<std::string,std::array<real,vfpic::nthreads>> diag;
    std::array<std::map<std::string,real>,vfpic::nthreads> diag2;
    std::map<std::string,real> sum;
    std::ofstream file;
    bool header;
    Diagnostics (const std::string&);
    ~Diagnostics ();
    void operator() (const GlobalVariables&, const LocalVectorField<real>&,
                     Barrier&, const int);
};

#endif /* defined(__vf_pic__diagnostics__) */
