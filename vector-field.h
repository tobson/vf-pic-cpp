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

template <typename T, int N1, int N2>
using VectorBase = ThreeVector<ScalarBase,T,N1,N2>;

template <typename T, int N1, int N2>
using VectorField = ThreeVector<ScalarField,T,N1,N2>;

template <typename T, int N1, int N2>
using VectorBaseView = ThreeVector<ScalarBaseView,T,N1,N2>;

template <typename T>
using GlobalVectorField = VectorField<T,vfpic::nz,vfpic::nx>;

template <typename T>
using LocalVectorField = VectorField<T,vfpic::mz,vfpic::mx>;

template <typename T>
class LocalVectorFieldView: public VectorBaseView<T,vfpic::mz,vfpic::mx>
{
    using base_class = VectorBaseView<T,vfpic::mz,vfpic::mx>;
    using scalar_class = ScalarBaseView<T,vfpic::mz,vfpic::mx>;
public:
    LocalVectorFieldView (GlobalVectorField<T>& global, int ithread):
    base_class (scalar_class (global.x, ithread),
                scalar_class (global.y, ithread),
                scalar_class (global.z, ithread))
    {
    }
    using base_class::operator=;
    using base_class::operator+=;
    using base_class::operator-=;
    using base_class::operator*=;
    using base_class::operator/=;
};

#endif
