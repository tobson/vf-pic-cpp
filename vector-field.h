//
//  vector-field.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_vector_field_h
#define vf_pic_vector_field_h

#include "scalar-field.h"
#include "three-vector.h"

/* Decleration */

template <typename T>
using GlobalVectorField = ThreeVector<GlobalScalarField<T>>;

template <typename T>
using LocalVectorField = ThreeVector<LocalScalarField<T>>;

template <typename T>
struct LocalVectorFieldView: public ThreeVector<LocalScalarFieldView<T>>
{
    LocalVectorFieldView (GlobalVectorField<T>& global, int ithread):
    ThreeVector<View> (View (global.x, ithread), View (global.y, ithread), View (global.z, ithread))
    {
    }
    using ThreeVector<LocalScalarFieldView<T>>::operator=;
    using ThreeVector<LocalScalarFieldView<T>>::operator+=;
    using ThreeVector<LocalScalarFieldView<T>>::operator-=;
    using ThreeVector<LocalScalarFieldView<T>>::operator*=;
    using ThreeVector<LocalScalarFieldView<T>>::operator/=;
private:
    typedef LocalScalarFieldView<T> View;
};

template <typename T>
struct VectorPair
{
    VectorPair (GlobalVectorField<T>& global, int ithread):
    global (global), local (LocalVectorFieldView<T> (global, ithread)),
    x (global.x, ithread), y (global.y, ithread), z (global.z, ithread)
    {
    }
    GlobalVectorField<T>& global;
    LocalVectorFieldView<T> local;
    ScalarPair<real> x, y, z;
};

#endif
