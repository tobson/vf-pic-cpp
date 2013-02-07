//
//  three-vector.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_h
#define vf_pic_three_vector_h

#include "scalar-field.h"

#include <functional>
#include <iostream>
#include <stdexcept>

/* Class decleration */

template <template <typename, int, int> class S, typename T, int N1, int N2>
class ThreeVector
{
private:
    typedef ScalarBase<T,N1,N2> base_class;
public:
    ThreeVector ()
    {
    }
    ThreeVector (const ThreeVector& other) = default;
    ThreeVector& operator= (const ThreeVector& other) = default;
    ThreeVector (ThreeVector&& other):
    x (std::move (other.x)),
    y (std::move (other.y)),
    z (std::move (other.z))
    {
    }
    ThreeVector& operator= (ThreeVector&& other) = delete;
    virtual ~ThreeVector () = default;
public:
    ThreeVector (const S<T,N1,N2>& a, const S<T,N1,N2>& b, const S<T,N1,N2>& c):
    x (a),
    y (b),
    z (c)
    {
    }
    ThreeVector (S<T,N1,N2>&& a, S<T,N1,N2>&& b, S<T,N1,N2>&& c):
    x (std::move (a)),
    y (std::move (b)),
    z (std::move (c))
    {
    }
private:
    using FieldOp = std::mem_fun1_t<base_class&,base_class,const base_class&>;
    using ValueOp = std::mem_fun1_t<base_class&,base_class,const T&>;
    inline void lift (ValueOp func, const T& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
    template <template <typename, int, int> class U>
    inline void lift (FieldOp func, const ThreeVector<U,T,N1,N2>& other)
    {
        static_assert (std::is_base_of<base_class,U<T,N1,N2>>::value, "");
        func (&x, other.x);
        func (&y, other.y);
        func (&z, other.z);
    }
    template <template <typename, int, int> class U>
    inline void lift (FieldOp func, const U<T,N1,N2>& other)
    {
        static_assert (std::is_base_of<base_class,U<T,N1,N2>>::value, "");
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
public:
    /* Assign */
    inline ThreeVector& operator= (const T& value)
    {
        lift (ValueOp (&base_class::operator=), value);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator= (const U& other)
    {
        lift (FieldOp (&base_class::operator=), other);
        return *this;
    }
    /* Add to */
    inline ThreeVector& operator+= (const T& value)
    {
        lift (ValueOp (&base_class::operator+=), value);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (const U& other)
    {
        lift (FieldOp (&base_class::operator+=), other);
        return *this;
    }
    /* Subtract from */
    inline ThreeVector& operator-= (const T& value)
    {
        lift (ValueOp (&base_class::operator-=), value);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator-= (const U& other)
    {
        lift (FieldOp (&base_class::operator-=), other);
        return *this;
    }
    /* Multiply with */
    inline ThreeVector& operator*= (const T& value)
    {
        lift (ValueOp (&base_class::operator*=), value);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (const U& other)
    {
        lift (FieldOp (&base_class::operator*=), other);
        return *this;
    }
    /* Divide by */
    inline ThreeVector& operator/= (const T& value)
    {
        lift (ValueOp (&base_class::operator/=), value);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator/= (const U& other)
    {
        lift (FieldOp (&base_class::operator/=), other);
        return *this;
    }
    inline S<T,N1,N2>& operator[] (int j)
    {
        switch (j)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                throw std::range_error ("Index out of bounds");
        }
    }
    inline const S<T,N1,N2>& operator[] (int j) const
    {
        switch (j)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                throw std::range_error ("Index out of bounds");
        }
    }
    friend std::ostream& operator<< (std::ostream& os, const ThreeVector& vector)
    {
        os << vector.x;
        os << vector.y;
        os << vector.z;
        return os;
    }

    S<T,N1,N2> x, y, z;
};

#endif
