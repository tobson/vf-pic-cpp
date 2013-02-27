//
//  sse.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 2/1/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "vector-field.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T, int N1, int N2>
struct VecBase
{
    using S = ScalarField<T,N1,N2>;
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
    std::vector<std::unique_ptr<ScalarField<T,N1,N2>>> components;
};

template <template <typename, int, int> class S, typename T, int N1, int N2>
struct VecTemplate: public VecBase<T,N1,N2>
{
    VecTemplate (S<T,N1,N2> *px, S<T,N1,N2> *py, S<T,N1,N2> *pz):
    VecBase<T,N1,N2> (px, py, pz),
    x (*static_cast<S<T,N1,N2>*> (VecBase<T,N1,N2>::components.at (0).get ())),
    y (*static_cast<S<T,N1,N2>*> (VecBase<T,N1,N2>::components.at (1).get ())),
    z (*static_cast<S<T,N1,N2>*> (VecBase<T,N1,N2>::components.at (2).get ()))
    {
        components.push_back (&x);
        components.push_back (&y);
        components.push_back (&z);
    }
    S<T,N1,N2>& operator[] (int j)
    {
        return *components[j];
    }
    S<T,N1,N2>& x;
    S<T,N1,N2>& y;
    S<T,N1,N2>& z;
private:
    std::vector<S<T,N1,N2>*> components;
};

template <typename T, int N1, int N2>
struct VecField3: public VecTemplate<NewScalarField,T,N1,N2>
{
    using S = NewScalarField<T,N1,N2>;
    VecField3 (): VecTemplate<NewScalarField,T,N1,N2> (new S, new S, new S)
    {
    }
};

struct VecView3: public VecTemplate<ScalarFieldView,real,vfpic::mz,vfpic::mx>
{
    using S = ScalarFieldView<real,vfpic::mz,vfpic::mx>;
    VecView3 (GlobalVectorField<real>& global, int ithread):
    VecTemplate<ScalarFieldView,real,vfpic::mz,vfpic::mx> (new S (global.x, ithread),
                                                          new S (global.y, ithread),
                                                          new S (global.z, ithread))
    {
    }
};

void test (const VecBase<real,vfpic::mz,vfpic::mx>& vector)
{
}

template <typename T, int N1, int N2>
void func (NewVectorField<T,N1,N2>& other)
{
}

template void func<real,vfpic::nz,vfpic::nx> (NewGlobalVectorField<real>&);

int main (int argc, const char * argv[])
{
    config::verbose = true;

    int pansen = 7;
    struct
    {
        int *edleff;
    } global;
    global.edleff = &pansen;
    printf ("edleff = %d\n", *global.edleff);

    NewGlobalVectorField<real> a;
    
    VecField3<real,vfpic::mz,vfpic::mx> v1, v2;
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