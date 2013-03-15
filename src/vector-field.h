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

template <typename T, uint Nz, uint Nx>
class VectorField
{
private:
    using Scalar = ScalarField<T,Nz,Nx>;
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
    Scalar& operator[] (uint j)
    {
        return *components[j];
    }
    const Scalar& operator[] (uint j) const
    {
        return *components[j];
    }
    real variance () const
    {
        return x.variance () + y.variance () + z.variance ();
    }
    real rms () const
    {
        return sqrt (this->variance());
    }
};

template <template <typename, uint, uint> class S, typename T, uint Nz, uint Nx>
class VectorTemplate: public VectorField<T,Nz,Nx>
{
private:
    using Scalar = S<T,Nz,Nx>;
    using Base = VectorField<T,Nz,Nx>;
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
    Scalar& operator[] (uint j)
    {
        return *components[j];
    }
    const Scalar& operator[] (uint j) const
    {
        return *components[j];
    }
    using Base::operator=;
};

template <typename T, uint Nz, uint Nx>
class NewVectorField: public VectorTemplate<NewScalarField,T,Nz,Nx>
{
private:
    using Scalar = NewScalarField<T,Nz,Nx>;
    using Base = VectorTemplate<NewScalarField,T,Nz,Nx>;
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

template <typename T, uint Nz, uint Nx>
class VectorFieldView: public VectorTemplate<ScalarFieldView,T,Nz,Nx>
{
private:
    using Scalar = ScalarFieldView<T,Nz,Nx>;
    using Base = VectorTemplate<ScalarFieldView,T,Nz,Nx>;
public:
    template <uint Mz, uint Mx>
    VectorFieldView (VectorField<T,Mz,Mx>& global, uint ithread):
    Base (new Scalar (global.x, ithread),
          new Scalar (global.y, ithread),
          new Scalar (global.z, ithread))
    {
    }
    template <uint Mz, uint Mx>
    VectorFieldView (const VectorField<T,Mz,Mx>& global, uint ithread):
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
