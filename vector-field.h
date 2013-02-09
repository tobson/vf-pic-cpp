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

#include <memory>
#include <vector>

/* Decleration */

template <typename T, int N1, int N2>
struct VectorBase
{
    using S = ScalarBase<T,N1,N2>;
    VectorBase (S *px, S *py, S *pz):
    x (*px),
    y (*py),
    z (*pz)
    {
        components.push_back (std::unique_ptr<S> (px));
        components.push_back (std::unique_ptr<S> (py));
        components.push_back (std::unique_ptr<S> (pz));
    }
    VectorBase () = delete;
    VectorBase (const VectorBase& other) = delete;
    VectorBase& operator= (const VectorBase& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        
        return *this;
    }
    VectorBase (VectorBase&& other) noexcept:
    x (other.x),
    y (other.y),
    z (other.z),
    components (std::move (other.components))
    {
    }
    virtual ~VectorBase () noexcept = default;
private:
    using FieldOp = std::mem_fun1_t<S&,S,const S&>;
    using ValueOp = std::mem_fun1_t<S&,S,const T&>;
    void lift (FieldOp func, const VectorBase& other)
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
    VectorBase& operator= (const T& value)
    {
        lift (ValueOp (&S::operator=), value);
        return *this;
    }
    VectorBase& operator= (const S& other)
    {
        lift (FieldOp (&S::operator=), other);
        return *this;
    }
    /* Add to */
    VectorBase& operator+= (const T& value)
    {
        lift (ValueOp (&S::operator+=), value);
        return *this;
    }
    template <class U>
    VectorBase& operator+= (const U& other)
    {
        lift (FieldOp (&S::operator+=), other);
        return *this;
    }
    /* Subtract from */
    inline VectorBase& operator-= (const T& value)
    {
        lift (ValueOp (&S::operator-=), value);
        return *this;
    }
    template <class U>
    inline VectorBase& operator-= (const U& other)
    {
        lift (FieldOp (&S::operator-=), other);
        return *this;
    }
    /* Multiply with */
    inline VectorBase& operator*= (const T& value)
    {
        lift (ValueOp (&S::operator*=), value);
        return *this;
    }
    template <class U>
    inline VectorBase& operator*= (const U& other)
    {
        lift (FieldOp (&S::operator*=), other);
        return *this;
    }
    /* Divide by */
    inline VectorBase& operator/= (const T& value)
    {
        lift (ValueOp (&S::operator/=), value);
        return *this;
    }
    template <class U>
    inline VectorBase& operator/= (const U& other)
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
    std::vector<std::unique_ptr<ScalarBase<T,N1,N2>>> components;
};

template <template <typename, int, int> class S, typename T, int N1, int N2>
struct VectorTemplate: public VectorBase<T,N1,N2>
{
    VectorTemplate (S<T,N1,N2> *px, S<T,N1,N2> *py, S<T,N1,N2> *pz):
    VectorBase<T,N1,N2> (px, py, pz),
    x (*static_cast<S<T,N1,N2>*> (VectorBase<T,N1,N2>::components.at (0).get ())),
    y (*static_cast<S<T,N1,N2>*> (VectorBase<T,N1,N2>::components.at (1).get ())),
    z (*static_cast<S<T,N1,N2>*> (VectorBase<T,N1,N2>::components.at (2).get ()))
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
    using VectorBase<T,N1,N2>::operator=;
private:
    std::vector<S<T,N1,N2>*> components;
};

template <typename T, int N1, int N2>
struct VectorField: public VectorTemplate<ScalarField,T,N1,N2>
{
    using S = ScalarField<T,N1,N2>;
    VectorField ():
    VectorTemplate<ScalarField,T,N1,N2> (new S, new S, new S)
    {
    }
    VectorField (const VectorField& other):
    VectorTemplate<ScalarField,T,N1,N2> (new S, new S, new S)
    {
        *this = other;
    }
    VectorField& operator= (const VectorField& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;

        return *this;
    }
    VectorField (VectorField&& other):
    VectorTemplate<ScalarField,T,N1,N2> (std::move (other))
    {
    }
    VectorField& operator= (VectorField&&) = delete;
    friend std::ostream& operator<< (std::ostream& os, const VectorField& vector)
    {
        os << vector.x;
        os << vector.y;
        os << vector.z;
        
        return os;
    }
    using VectorTemplate<ScalarField,T,N1,N2>::operator=;
};

template <typename T>
using GlobalVectorField = VectorField<T,vfpic::nz,vfpic::nx>;

template <typename T>
using LocalVectorField = VectorField<T,vfpic::mz,vfpic::mx>;

template <typename T>
struct LocalVectorFieldView: public VectorTemplate<ScalarBaseView,T,vfpic::mz,vfpic::mx>
{
    using S = ScalarBaseView<T,vfpic::mz,vfpic::mx>;
    LocalVectorFieldView (GlobalVectorField<T>& global, int ithread):
    VectorTemplate<ScalarBaseView,T,vfpic::mz,vfpic::mx> (new S (global.x, ithread),
                                                          new S (global.y, ithread),
                                                          new S (global.z, ithread))
    {
    }
};

#endif
