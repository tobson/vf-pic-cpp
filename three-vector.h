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
    static const int nz = S::nz;
    static const int nx = S::nx;
    typedef ScalarBase<value_type,nz,nx> Base;
public:
    ThreeVector ()
    {
        static_assert (std::is_base_of<Base,S>::value, "");
    }
    ThreeVector (const S& a, const S& b, const S& c): x (a), y (b), z (c)
    {
        static_assert (std::is_base_of<Base,S>::value, "");
    }
    ThreeVector (S&& a, S&& b, S&& c): x (std::move (a)), y (std::move (b)), z (std::move (c))
    {
        static_assert (std::is_base_of<Base,S>::value, "");
    }
private:
    template <typename F, class U>
    inline void lift (F function, const ThreeVector<U>& other)
    {
        static_assert (std::is_base_of<Base,U>::value, "");
        function (x, other.x);
        function (y, other.y);
        function (z, other.z);
    }
    template <typename F>
    inline void lift (F function, const Base& other)
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
        lift (std::mem_fn<Base&,Base,const value_type&> (&Base::operator=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator= (const U& other)
    {
        lift (std::mem_fn<Base&,Base,const Base&> (&Base::operator=), other);
        return *this;
    }
    inline ThreeVector& operator+= (const value_type& other)
    {
        lift (std::mem_fn<Base&,Base,const value_type&> (&Base::operator+=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (const U& other)
    {
        lift (std::mem_fn<Base&,Base,const Base&> (&Base::operator+=), other);
        return *this;
    }
    inline ThreeVector& operator-= (const value_type& other)
    {
        lift (std::mem_fn<Base&,Base,const value_type&> (&Base::operator-=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator-= (const U& other)
    {
        lift (std::mem_fn<Base&,Base,const Base&> (&Base::operator-=), other);
        return *this;
    }
    inline ThreeVector& operator*= (const value_type& other)
    {
        lift (std::mem_fn<Base&,Base,const value_type&> (&Base::operator*=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (const U& other)
    {
        lift (std::mem_fn<Base&,Base,const Base&> (&Base::operator*=), other);
        return *this;
    }
    inline ThreeVector& operator/= (const value_type& other)
    {
        lift (std::mem_fn<Base&,Base,const value_type&> (&Base::operator/=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator/= (const U& other)
    {
        lift (std::mem_fn<Base&,Base,const Base&> (&Base::operator/=), other);
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
    template <class U>
    U lift (U (*func)(S&))
    {
        func (x);
        func (y);
        func (z);
    }

    S x, y, z;
};

#endif
