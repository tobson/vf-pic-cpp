//
//  test.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"
#include "boundaries.h"
#include "config.h"
#include "global.h"
#include "scalar-field.h"
#include "vector-field.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <vector>

struct Grid
{
    Grid ()
    {
        for (int k = 0; k < global::nz + 2; ++k)
        for (int i = 0; i < global::nx + 2; ++i)
        {
            x(k,i) = config::x0 + (i - 0.5)*global::dx;
            z(k,i) = config::z0 + (k - 0.5)*global::dz;
        }
    }
    GlobalScalarField<real> x, z;
};

struct Fields
{
    Fields (const Grid &grid)
    {
        const real two_pi = 2.0*pi;

        using config::ikx;
        using config::ikz;
        
        const GlobalScalarField<real> &x = grid.x;
        const GlobalScalarField<real> &z = grid.z;
                
        for (int k = 0; k < global::nz + 2; k++)
        for (int i = 0; i < global::nx + 2; i++)
        {
            rho(k,i) = sin(two_pi*ikx*x(k,i))*cos(two_pi*ikz*z(k,i));
        }
        phi = 0.0;
        phi0 = 0.0;
        
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < global::nz + 2; k++)
            for (int i = 0; i < global::nx + 2; i++)
            {
                J[j](k,i) = (j + 2)*sin(two_pi*ikx*x(k,i))*cos(two_pi*ikz*z(k,i));
            }
            A[j] = 0.0;
            A0[j] = 0.0;
        }
    }
    GlobalScalarField<real> rho, phi, phi0;
    GlobalVectorField<real> J, A, A0;
};

void iteration (Fields &fields, const int niter)
{
    const real dx12 = real (1.0)/(global::dx*global::dx);
    const real dz12 = real (1.0)/(global::dz*global::dz);
    const real fac = real (0.5)/(dx12 + dz12);

    const GlobalScalarField<real>& rho = fields.rho;

    GlobalScalarField<real>& phi = fields.phi;
    GlobalScalarField<real>& phi0 = fields.phi0;

    const GlobalVectorField<real>& J = fields.J;

    GlobalVectorField<real>& A = fields.A;
    GlobalVectorField<real>& A0 = fields.A0;

    for (int dummy = 0; dummy < niter; dummy++)
    {
        for (int k = 1; k <= global::nz; ++k)
        for (int i = 1; i <= global::nx; ++i)
        {
            phi0(k,i) = fac*(dx12*(phi(k  ,i-1) + phi(k  ,i+1)) +
                             dz12*(phi(k-1,i  ) + phi(k+1,i  )) + rho(k,i));
        }
        for (int j = 0; j < 3; j++)
        {
            for (int k = 1; k <= global::nz; ++k)
            for (int i = 1; i <= global::nx; ++i)
            {
                A0[j](k,i) = fac*(dx12*(A[j](k  ,i-1) + A[j](k  ,i+1)) +
                                  dz12*(A[j](k-1,i  ) + A[j](k+1,i  )) + J[j](k,i));
            }
        }

        boundaryCondition (phi0);
        boundaryCondition (A0);
        
        for (int k = 1; k <= global::nz; ++k)
        for (int i = 1; i <= global::nx; ++i)
        {
            phi(k,i) = fac*(dx12*(phi0(k  ,i-1) + phi0(k  ,i+1)) +
                            dz12*(phi0(k-1,i  ) + phi0(k+1,i  )) + rho(k,i));
        }
        for (int j = 0; j < 3; j++)
        {
            for (int k = 1; k <= global::nz; ++k)
            for (int i = 1; i <= global::nx; ++i)
            {
                A[j](k,i) = fac*(dx12*(A0[j](k  ,i-1) + A0[j](k  ,i+1)) +
                                 dz12*(A0[j](k-1,i  ) + A0[j](k+1,i  )) + J[j](k,i));
            }
        }

        boundaryCondition (phi);
        boundaryCondition (A);
    }
}

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
        std::cout << e.what () << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what () << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "Unknown exception" << std::endl;
        return 1;
    }

    Grid grid;

    std::ofstream myfile;
    myfile.open (srcdir + "/jacobi.dat", std::ios::binary);

    const int niter = 10;

    {
        Fields fields (grid);

        auto t1 = std::chrono::high_resolution_clock::now();
        iteration (fields, niter);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "Serial run: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count () << " ms" << std::endl;

        myfile << fields.rho;
        myfile << fields.J;
        myfile << fields.phi;
        myfile << fields.A;
    }

    myfile.close ();

    return 0;
}