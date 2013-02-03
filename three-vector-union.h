//
//  three-vector-stupid-intel.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/2/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_stupid_intel_h
#define vf_pic_three_vector_stupid_intel_h

#include <array>
#include <functional>
#include <iostream>
#include <stdexcept>

/* Class decleration */

template <class S>
class ThreeVector
{
public:
    union
    {
        struct
        {
            S x, y, z;
        };
        std::array<S,3> components;
    };
private:
    typedef typename S::value_type value_type;
    typedef ScalarBase<value_type,S::nz,S::nx> Scalar;
public:
    ThreeVector (): components ()
    {
    }
    ThreeVector (const ThreeVector& other): components (other.components)
    {
    }
    ThreeVector& operator= (const ThreeVector& other)
    {
        components = other.components;
    }
    ThreeVector (ThreeVector&& other): components (std::move (other.components))
    {
    }
    ThreeVector& operator= (ThreeVector&& other)
    {
        components = std::move (other.components);
    }
    ~ThreeVector ()
    {
        x.~S ();
        y.~S ();
        z.~S ();
    }
    ThreeVector (const S& a, const S& b, const S& c):
    x (a),
    y (b),
    z (c)
    {
    }
    ThreeVector (S&& a, S&& b, S&& c):
    x (std::move (a)),
    y (std::move (b)),
    z (std::move (c))
    {
    }
private:
    template <typename F, class U>
    inline void lift (F function, const ThreeVector<U>& other)
    {
        static_assert (std::is_base_of<Scalar,S>::value, "");
        static_assert (std::is_base_of<Scalar,U>::value, "");
        function (x, other.x);
        function (y, other.y);
        function (z, other.z);
    }
    template <typename F>
    inline void lift (F function, const Scalar& other)
    {
        function (x, other);
        function (y, other);
        function (z, other);
    }
    template <typename F>
    inline void lift (F function, const value_type& other)
    {
        function (x, other);
        function (y, other);
        function (z, other);
    }
public:
    inline ThreeVector& operator= (const value_type& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const value_type&> (&Scalar::operator=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator= (const U& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const Scalar&> (&Scalar::operator=), other);
        return *this;
    }
    inline ThreeVector& operator+= (const value_type& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const value_type&> (&Scalar::operator+=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (const U& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const Scalar&> (&Scalar::operator+=), other);
        return *this;
    }
    inline ThreeVector& operator-= (const value_type& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const value_type&> (&Scalar::operator-=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator-= (const U& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const Scalar&> (&Scalar::operator-=), other);
        return *this;
    }
    inline ThreeVector& operator*= (const value_type& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const value_type&> (&Scalar::operator*=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (const U& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const Scalar&> (&Scalar::operator*=), other);
        return *this;
    }
    inline ThreeVector& operator/= (const value_type& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const value_type&> (&Scalar::operator/=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator/= (const U& other)
    {
        lift (std::mem_fn<Scalar&,Scalar,const Scalar&> (&Scalar::operator/=), other);
        return *this;
    }
    inline S& operator[] (int j)
    {
        return components[j];
    }
    inline const S& operator[] (int j) const
    {
        return components[j];
    }
    friend std::ostream& operator<< (std::ostream& os, const ThreeVector& vector)
    {
        os << vector.x;
        os << vector.y;
        os << vector.z;
        return os;
    }
};

#endif
