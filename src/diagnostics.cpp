//
//  diagnostics.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 3/10/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "diagnostics.h"

#include "particles.h"
#include "vector-field.h"

using namespace vfpic;
using namespace config;

Diagnostics::Diagnostics (const std::string& filename): file (filename), header (true)
{
    for (uint ithread = 0; ithread < nthreads; ++ithread)
    {
        std::map<std::string,real>& map = diag2.at (ithread);
        map["mvx"] = 0.0;
        map["mvy"] = 0.0;
        map["mvz"] = 0.0;
        map["ekin"] = 0.0;
        map["eth"] = 0.0;
        map["emag"] = 0.0;
    }
    for (uint ithread = 0; ithread < nthreads; ++ithread)
    {
        std::map<std::string,real>& map = diag2.at (ithread);
        for (auto iter = map.begin (); iter != map.end (); ++iter)
        {
            sum[iter->first] = 0.0;
        }
    }
    header = false;
    file << "# ";
    std::string sep = "";
    for (auto iter = sum.begin (); iter != sum.end (); ++iter)
    {
        file << sep << iter->first;
        sep = "\t";
    }
    file << std::endl;
    std::cout << "filename = " << filename << std::endl;
}

Diagnostics::~Diagnostics ()
{
    file.close ();
}

void Diagnostics::operator() (const GlobalVariables& global,
                              const LocalVectorField<real>& H, const long long it,
                              Barrier& barrier, const uint ithread)
{
    const LocalParticlesView particles (global.particles, ithread);
    const LocalScalarFieldView<real> rho (global.rho, ithread);
    const LocalVectorFieldView<real> ruu (global.ruu, ithread);
    
    const real fnorm = 1.0/real (nx*nz);
    const real pnorm = fnorm*rho0/real (npc);
    
    std::map<std::string,real>& map = diag2.at (ithread);

    for (auto iter = map.begin (); iter != map.end (); ++iter) iter->second = 0.0;

#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
    for (auto p = particles.begin (); p != particles.end (); ++p)
    {
        map["mvx"] += p->vx;
        map["mvy"] += p->vy;
        map["mvz"] += p->vz;
        map["ekin"] += p->vx*p->vx + p->vy*p->vy + p->vz*p->vz;
    }
    map["mvx"] *= pnorm;
    map["mvy"] *= pnorm;
    map["mvz"] *= pnorm;
    map["ekin"] *= 0.5*pnorm;
    
    for (uint j = 0; j < 3; ++j)
    {
        for (uint k = LocalScalarField<real>::k1;
                  k < LocalScalarField<real>::k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = LocalScalarField<real>::i1;
                  i < LocalScalarField<real>::i2; ++i)
        {
            map["eth"] += pow (ruu[j] (k,i), 2)/rho (k,i);
            map["emag"] += pow (H[j] (k,i), 2);
       }
    }
    map["eth"] *= 0.5*fnorm;
    map["emag"] *= 0.5*fnorm;
    
    if (barrier.wait ())
    {
        for (auto iter = sum.begin (); iter != sum.end (); ++iter)
        {
            iter->second = 0.0;
        }
        for (uint ithread = 0; ithread < nthreads; ++ithread)
        {
            std::map<std::string,real>& map = diag2.at (ithread);
            for (auto iter = map.begin (); iter != map.end (); ++iter)
            {
                sum[iter->first] += iter->second;
            }
        }
        std::string sep = "";
        for (auto iter = sum.begin (); iter != sum.end (); ++iter)
        {
            file << sep << iter->second;
            sep = "\t";
        }
        file << std::endl;
        file.flush ();
        std::cout << "it = " << it << ": Diagnostics written" << std::endl;
    }
    barrier.wait ();
}
