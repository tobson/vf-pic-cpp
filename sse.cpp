//
//  sse.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/1/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "vector-field.h"

#include <iostream>
#include <vector>

const int nx = 16, nz = 32;

struct VecBase
{
    using S = ScalarBase<real,nz,nx>;
    VecBase (S *px, S *py, S *pz): x (*px), y (*py), z (*pz)
    {
        components.push_back (std::unique_ptr<S> (px));
        components.push_back (std::unique_ptr<S> (py));
        components.push_back (std::unique_ptr<S> (pz));
    }
    VecBase& operator+= (const VecBase& other)
    {
        std::cout << "Pansen!\n";
        
        x += other.x;
        y += other.y;
        z += other.z;
        
        return *this;
    }
    S& operator[] (int j)
    {
        return *components[j];
    }
    S& x; S& y; S& z;
protected:
    std::vector<std::unique_ptr<ScalarBase<real,nz,nx>>> components;
};

template <class S>
struct VecTemplate: public VecBase
{
    VecTemplate (S *px, S *py, S *pz): VecBase (px, py, pz),
    x (*static_cast<S*> (components.at (0).get ())),
    y (*static_cast<S*> (components.at (1).get ())),
    z (*static_cast<S*> (components.at (2).get ()))
    {
        _components.push_back (&x);
        _components.push_back (&y);
        _components.push_back (&z);
    }
    S& operator[] (int j)
    {
        return *_components[j];
    }
    S &x, &y, &z;
private:
    std::vector<S*> _components;
};

struct VecField3: public VecTemplate<ScalarField<real,nz,nx>>
{
    using S = ScalarField<real,nz,nx>;
    VecField3 (): VecTemplate<S> (new S, new S, new S)
    {
    }
};

struct VecView3: public VecTemplate<ScalarBaseView<real,nz,nx>>
{
    using S = ScalarBaseView<real,nz,nx>;
    VecView3 (GlobalVectorField<real>& global, int ithread):
    VecTemplate<S> (new S (global.x, ithread),
                    new S (global.y, ithread),
                    new S (global.z, ithread))
    {
    }
};

void test (const VecBase& vector)
{
}

int main (int argc, const char * argv[])
{
    config::verbose = true;

    GlobalVectorField<real> a;
    
    VecField3 v1, v2;
    v1 += v2;
    test (v1);
    
    VecView3 view (a, 0);
    
    view += v1;
    
    test (view);
//    v1.x (3,7) = 7.2;
//    std::cout << v1.x (3,7) << std::endl;
//    GlobalVectorField<real> a;

//    ScalarField<real,nz,nx> *x = new ScalarField<real,nz,nx>;
//    ScalarBase<real,nz,nx> *y = x;
//    delete y;
    return 0;
}