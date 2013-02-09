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

#include <memory>
#include <vector>

/* Decleration */

template <typename T, int N1, int N2>
struct VectorField
{
    using S = ScalarField<T,N1,N2>;
    VectorField (S *px, S *py, S *pz):
    x (*px),
    y (*py),
    z (*pz)
    {
        components.push_back (std::unique_ptr<S> (px));
        components.push_back (std::unique_ptr<S> (py));
        components.push_back (std::unique_ptr<S> (pz));
    }
    VectorField () = delete;
    VectorField (const VectorField& other) = delete;
    VectorField& operator= (const VectorField& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        
        return *this;
    }
    VectorField (VectorField&& other) noexcept:
    x (other.x),
    y (other.y),
    z (other.z),
    components (std::move (other.components))
    {
    }
    virtual ~VectorField () noexcept = default;
private:
    using FieldOp = std::mem_fun1_t<S&,S,const S&>;
    using ValueOp = std::mem_fun1_t<S&,S,const T&>;
    void lift (FieldOp func, const VectorField& other)
    {
        func (&x, other.x);
        func (&y, other.y);
        func (&z, other.z);
    }
    void lift (FieldOp func, const S& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
    void lift (ValueOp func, const T& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
public:
    /* Assign */
    VectorField& operator= (const T& value)
    {
        lift (ValueOp (&S::operator=), value);
        return *this;
    }
    VectorField& operator= (const S& other)
    {
        lift (FieldOp (&S::operator=), other);
        return *this;
    }
    /* Add to */
    VectorField& operator+= (const T& value)
    {
        lift (ValueOp (&S::operator+=), value);
        return *this;
    }
    template <class U>
    VectorField& operator+= (const U& other)
    {
        lift (FieldOp (&S::operator+=), other);
        return *this;
    }
    /* Subtract from */
    inline VectorField& operator-= (const T& value)
    {
        lift (ValueOp (&S::operator-=), value);
        return *this;
    }
    template <class U>
    inline VectorField& operator-= (const U& other)
    {
        lift (FieldOp (&S::operator-=), other);
        return *this;
    }
    /* Multiply with */
    inline VectorField& operator*= (const T& value)
    {
        lift (ValueOp (&S::operator*=), value);
        return *this;
    }
    template <class U>
    inline VectorField& operator*= (const U& other)
    {
        lift (FieldOp (&S::operator*=), other);
        return *this;
    }
    /* Divide by */
    inline VectorField& operator/= (const T& value)
    {
        lift (ValueOp (&S::operator/=), value);
        return *this;
    }
    template <class U>
    inline VectorField& operator/= (const U& other)
    {
        lift (FieldOp (&S::operator/=), other);
        return *this;
    }
    S& operator[] (int j)
    {
        return *components[j];
    }
    const S& operator[] (int j) const
    {
        return *components[j];
    }
    S& x; S& y; S& z;
protected:
    std::vector<std::unique_ptr<ScalarField<T,N1,N2>>> components;
};

template <template <typename, int, int> class S, typename T, int N1, int N2>
struct VectorTemplate: public VectorField<T,N1,N2>
{
    VectorTemplate (S<T,N1,N2> *px, S<T,N1,N2> *py, S<T,N1,N2> *pz):
    VectorField<T,N1,N2> (px, py, pz),
    x (*static_cast<S<T,N1,N2>*> (VectorField<T,N1,N2>::components.at (0).get ())),
    y (*static_cast<S<T,N1,N2>*> (VectorField<T,N1,N2>::components.at (1).get ())),
    z (*static_cast<S<T,N1,N2>*> (VectorField<T,N1,N2>::components.at (2).get ()))
    {
        components.push_back (&x);
        components.push_back (&y);
        components.push_back (&z);
    }
    S<T,N1,N2>& operator[] (int j)
    {
        return *components[j];
    }
    const S<T,N1,N2>& operator[] (int j) const
    {
        return *components[j];
    }
    S<T,N1,N2>& x;
    S<T,N1,N2>& y;
    S<T,N1,N2>& z;
    using VectorField<T,N1,N2>::operator=;
private:
    std::vector<S<T,N1,N2>*> components;
};

template <typename T, int N1, int N2>
struct NewVectorField: public VectorTemplate<NewScalarField,T,N1,N2>
{
    using S = NewScalarField<T,N1,N2>;
    NewVectorField ():
    VectorTemplate<NewScalarField,T,N1,N2> (new S, new S, new S)
    {
    }
    NewVectorField (const NewVectorField& other):
    VectorTemplate<NewScalarField,T,N1,N2> (new S, new S, new S)
    {
        *this = other;
    }
    NewVectorField& operator= (const NewVectorField& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }
    NewVectorField (NewVectorField&& other):
    VectorTemplate<NewScalarField,T,N1,N2> (std::move (other))
    {
    }
    NewVectorField& operator= (NewVectorField&&) = delete;
    friend std::ostream& operator<< (std::ostream& os, const NewVectorField& vector)
    {
        os << vector.x;
        os << vector.y;
        os << vector.z;
        
        return os;
    }
    using VectorTemplate<NewScalarField,T,N1,N2>::operator=;
};

template <typename T, int N1, int N2>
struct VectorFieldView: public VectorTemplate<ScalarFieldView,T,N1,N2>
{
    using S = ScalarFieldView<T,N1,N2>;
    template <int M1, int M2>
    VectorFieldView (VectorField<T,M1,M2>& global, int ithread):
    VectorTemplate<ScalarFieldView,T,N1,N2> (new S (global.x, ithread),
                                             new S (global.y, ithread),
                                             new S (global.z, ithread))
    {
    }
};

template <typename T>
using GlobalVectorField = VectorField<T,vfpic::nz,vfpic::nx>;

template <typename T>
using LocalVectorField = VectorField<T,vfpic::mz,vfpic::mx>;

template <typename T>
using NewGlobalVectorField = NewVectorField<T,vfpic::nz,vfpic::nx>;

template <typename T>
using NewLocalVectorField = NewVectorField<T,vfpic::mz,vfpic::mx>;
        
template <typename T>
using LocalVectorFieldView = VectorFieldView<T,vfpic::mz,vfpic::mx>;

#endif
