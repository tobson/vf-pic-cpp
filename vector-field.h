//
//  vector-field.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_vector_field_h
#define vf_pic_vector_field_h

#include <memory>
#include <vector>

#include "scalar-field.h"
#include "three-vector.h"

/* Decleration */

template <typename T, int N1, int N2>
class VectorField
{
private:
    using Scalar = ScalarField<T,N1,N2>;
public:
    Scalar& x;
    Scalar& y;
    Scalar& z;
protected:
    std::vector<std::unique_ptr<Scalar>> components;

    VectorField (Scalar *px, Scalar *py, Scalar *pz):
    x (*px),
    y (*py),
    z (*pz)
    {
        components.push_back (std::unique_ptr<Scalar> (px));
        components.push_back (std::unique_ptr<Scalar> (py));
        components.push_back (std::unique_ptr<Scalar> (pz));
    }
    virtual ~VectorField () noexcept = default;
public:
    VectorField& operator= (const VectorField& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        
        return *this;
    }
private:
    using FieldOp = std::mem_fun1_t<Scalar&,Scalar,const Scalar&>;
    using ValueOp = std::mem_fun1_t<Scalar&,Scalar,const T&>;
    
    template <class U, class Dummy> struct SelectDummy { using wrapper = FieldOp; };
    template <class Dummy> struct SelectDummy<const T&, Dummy> { using wrapper = ValueOp; };
    template <class Dummy> struct SelectDummy<const Vector<T>&, Dummy> { using wrapper = ValueOp; };
    template <class U> using Select = SelectDummy<U,void>;
    
    void lift (FieldOp func, const VectorField& other)
    {
        func (&x, other.x);
        func (&y, other.y);
        func (&z, other.z);
    }
    void lift (FieldOp func, const Scalar& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
    void lift (ValueOp func, const Vector<T>& other)
    {
        func (&x, other.x);
        func (&y, other.y);
        func (&z, other.z);
    }
    void lift (ValueOp func, const T& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
public:
    /* Assign */
    template <class U>
    VectorField& operator= (const U& value)
    {
        lift (typename Select<const U&>::wrapper (&Scalar::operator=), value);
        return *this;
    }
    /* Add to */
    template <class U>
    VectorField& operator+= (const U& other)
    {
        lift (typename Select<const U&>::wrapper (&Scalar::operator+=), other);
        return *this;
    }
    /* Subtract from */
    template <class U>
    inline VectorField& operator-= (const U& other)
    {
        lift (typename Select<const U&>::wrapper (&Scalar::operator-=), other);
        return *this;
    }
    /* Multiply with */
    template <class U>
    inline VectorField& operator*= (const U& other)
    {
        lift (typename Select<const U&>::wrapper (&Scalar::operator*=), other);
        return *this;
    }
    /* Divide by */
    template <class U>
    inline VectorField& operator/= (const U& other)
    {
        lift (typename Select<const U&>::wrapper (&Scalar::operator/=), other);
        return *this;
    }
    /* Index operators */
    Scalar& operator[] (int j)
    {
        return *components[j];
    }
    const Scalar& operator[] (int j) const
    {
        return *components[j];
    }
};

template <template <typename, int, int> class S, typename T, int N1, int N2>
class VectorTemplate: public VectorField<T,N1,N2>
{
private:
    using Scalar = S<T,N1,N2>;
    using Base = VectorField<T,N1,N2>;
public:
    Scalar& x;
    Scalar& y;
    Scalar& z;
private:
    std::vector<Scalar*> components;
protected:
    VectorTemplate (Scalar *px, Scalar *py, Scalar *pz): Base (px, py, pz),
    x (*px),
    y (*py),
    z (*pz)
    {
        components.push_back (px);
        components.push_back (py);
        components.push_back (pz);
    }
public:
    VectorTemplate& operator= (const VectorTemplate& other)
    {
        Base::operator= (other);
        return *this;
    }
    virtual ~VectorTemplate () noexcept = default;
    Scalar& operator[] (int j)
    {
        return *components[j];
    }
    const Scalar& operator[] (int j) const
    {
        return *components[j];
    }
    using Base::operator=;
};

template <typename T, int N1, int N2>
class NewVectorField: public VectorTemplate<NewScalarField,T,N1,N2>
{
private:
    using Scalar = NewScalarField<T,N1,N2>;
    using Base = VectorTemplate<NewScalarField,T,N1,N2>;
public:
    NewVectorField ():
    Base (new Scalar, new Scalar, new Scalar)
    {
    }
    NewVectorField (const NewVectorField& other):
    Base (new Scalar, new Scalar, new Scalar)
    {
        *this = other;
    }
    friend std::ostream& operator<< (std::ostream& os, const NewVectorField& vector)
    {
        os << vector.x;
        os << vector.y;
        os << vector.z;
        
        return os;
    }
    friend std::istream& operator>> (std::istream& is, NewVectorField& vector)
    {
        is >> vector.x;
        is >> vector.y;
        is >> vector.z;

        return is;
    }
    using Base::operator=;
};

template <typename T, int N1, int N2>
class VectorFieldView: public VectorTemplate<ScalarFieldView,T,N1,N2>
{
private:
    using Scalar = ScalarFieldView<T,N1,N2>;
    using Base = VectorTemplate<ScalarFieldView,T,N1,N2>;
public:
    template <int M1, int M2>
    VectorFieldView (VectorField<T,M1,M2>& global, int ithread):
    Base (new Scalar (global.x, ithread),
          new Scalar (global.y, ithread),
          new Scalar (global.z, ithread))
    {
    }
    using Base::operator=;
};

template <typename T = real>
using GlobalVectorField = VectorField<T,vfpic::nz,vfpic::nx>;

template <typename T = real>
using LocalVectorField = VectorField<T,vfpic::mz,vfpic::mx>;

template <typename T = real>
using NewGlobalVectorField = NewVectorField<T,vfpic::nz,vfpic::nx>;

template <typename T = real>
using NewLocalVectorField = NewVectorField<T,vfpic::mz,vfpic::mx>;
        
template <typename T = real>
using LocalVectorFieldView = VectorFieldView<T,vfpic::mz,vfpic::mx>;

#endif
