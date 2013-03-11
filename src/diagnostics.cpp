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
    for (int ithread = 0; ithread < nthreads; ++ithread)
    {
        std::map<std::string,real>& map = diag2.at (ithread);
        map["mvx"] = 0.0;
        map["mvy"] = 0.0;
        map["mvz"] = 0.0;
        map["ekin"] = 0.0;
        map["eth"] = 0.0;
        map["emag"] = 0.0;
    }
    for (int ithread = 0; ithread < nthreads; ++ithread)
    {
        std::map<std::string,real>& map = diag2.at (ithread);
        for (auto it = map.begin (); it != map.end (); ++it)
        {
            sum[it->first] = 0.0;
        }
    }
    header = false;
    file << "# ";
    std::string sep = "";
    for (auto it = sum.begin (); it != sum.end (); ++it)
    {
        file << sep << it->first;
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
                              Barrier& barrier, const int ithread)
{
    const LocalParticlesView particles (global.particles, ithread);
    const LocalScalarFieldView<real> rho (global.rho, ithread);
    const LocalVectorFieldView<real> ruu (global.ruu, ithread);
    
    const real fnorm = 1.0/real (mx*mz);
    const real pnorm = fnorm*rho0/real (npc);
    
    std::map<std::string,real>& map = diag2.at (ithread);

    for (auto it = map.begin (); it != map.end (); ++it) it->second = 0.0;

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
    
    
    for (int j = 0; j < 3; ++j)
    {
        for (int k = 1; k <= mz; ++k)
        for (int i = 1; i <= mx; ++i)
        {
            map["eth"] += pow (ruu[j] (k,i), 2)/rho (k,i);
            map["emag"] += pow (H[j] (k,i), 2);
       }
    }
    map["eth"] *= 0.5*fnorm;
    map["emag"] *= 0.5*fnorm;
    
    if (barrier.wait ())
    {
        for (auto it = sum.begin (); it != sum.end (); ++it)
        {
            it->second = 0.0;
        }
        for (int ithread = 0; ithread < nthreads; ++ithread)
        {
            std::map<std::string,real>& map = diag2.at (ithread);
            for (auto it = map.begin (); it != map.end (); ++it)
            {
                sum[it->first] += it->second;
            }
        }
        std::string sep = "";
        for (auto it = sum.begin (); it != sum.end (); ++it)
        {
            file << sep << it->second;
            sep = "\t";
        }
        file << std::endl;
        file.flush ();
        std::cout << "it = " << it << ": Diagnostics written" << std::endl;
    }
    barrier.wait ();
}
