//
//  three-vector.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_h
#define vf_pic_three_vector_h

#include <functional>
#include <iostream>
#include <stdexcept>

/* Class decleration */

template <class S>
class ThreeVector
{
private:
    typedef typename S::value_type value_type;
    typedef ScalarBase<value_type,S::nz,S::nx> Scalar;
public:
    ThreeVector ()
    {
        static_assert (std::is_base_of<Scalar,S>::value, "");
    }
    ThreeVector (const S& a, const S& b, const S& c): x (a), y (b), z (c)
    {
        static_assert (std::is_base_of<Scalar,S>::value, "");
    }
    ThreeVector (S&& a, S&& b, S&& c): x (std::move (a)), y (std::move (b)), z (std::move (c))
    {
        static_assert (std::is_base_of<Scalar,S>::value, "");
    }
private:
    using Op = std::mem_fun1_t<Scalar&,Scalar,const Scalar&>;
    template <class U>
    inline void lift (Op func, const ThreeVector<U>& other)
    {
        static_assert (std::is_base_of<Scalar,U>::value, "");
        func (&x, other.x);
        func (&y, other.y);
        func (&z, other.z);
    }
    inline void lift (Op func, const Scalar& other)
    {
        func (&x, other);
        func (&y, other);
        func (&z, other);
    }
public:
    template <class U>
    inline ThreeVector& operator= (const U& other)
    {
        lift (Op (&Scalar::operator=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (const U& other)
    {
        lift (Op (&Scalar::operator+=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator-= (const U& other)
    {
        lift (Op (&Scalar::operator-=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (const U& other)
    {
        lift (Op (&Scalar::operator*=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator/= (const U& other)
    {
        lift (Op (&Scalar::operator/=), other);
        return *this;
    }
    inline S& operator[] (int j)
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
    inline const S& operator[] (int j) const
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

    S x, y, z;
};

#endif
