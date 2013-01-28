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

template <typename T>
using GlobalVectorField = ThreeVector<GlobalScalarField<T>>;

template <typename T>
class LocalVectorField: public ThreeVector<LocalScalarField<T>>
{
public:
    using ThreeVector<LocalScalarField<T>>::operator=;
};

/* Decleration */

template <typename T>
class LocalVectorFieldView: public ThreeVector<LocalScalarFieldView<T>>
{
private:
    typedef LocalScalarFieldView<T> View;
public:
    LocalVectorFieldView (GlobalVectorField<T>&, int);
    using ThreeVector<LocalScalarFieldView<T>>::operator=;
};

/* Implementation */

template <typename T>
LocalVectorFieldView<T>::LocalVectorFieldView (GlobalVectorField<T>& global, int ithread):
    ThreeVector<View> (View (global.x, ithread), View (global.y, ithread), View (global.z, ithread))
{
}

#endif
