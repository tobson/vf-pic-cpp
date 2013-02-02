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
    typedef typename S::value_type T;
    static const int N1 = S::nz;
    static const int N2 = S::nx;
    typedef ScalarBase<T,N1,N2> B;
public:
    ThreeVector ()
    {
        static_assert (std::is_base_of<B,S>::value, "");
    }
    ThreeVector (const S& a, const S& b, const S& c): x (a), y (b), z (c)
    {
        static_assert (std::is_base_of<B,S>::value, "");
    }
    ThreeVector (S&& a, S&& b, S&& c): x (std::move (a)), y (std::move (b)), z (std::move (c))
    {
        static_assert (std::is_base_of<B,S>::value, "");
    }
private:
    template <typename F, class U>
    inline void lift (F function, const ThreeVector<U>& other)
    {
        static_assert (std::is_base_of<B,U>::value, "");
        function (x, other.x);
        function (y, other.y);
        function (z, other.z);
    }
    template <typename F>
    inline void lift (F function, const B& other)
    {
        function (x, other);
        function (y, other);
        function (z, other);
    }
    template <typename F>
    inline void lift (F function, const T& other)
    {
        function (x, other);
        function (y, other);
        function (z, other);
    }
public:
    
    template <class U>
    inline ThreeVector& operator= (ThreeVector<U>& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator=), other);
        return *this;
    }
    inline ThreeVector& operator= (B& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator=), other);
        return *this;
    }
    inline ThreeVector& operator= (const T other)
    {
        lift (std::mem_fn<B&,B,const T&> (&B::operator=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (ThreeVector<U>& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator+=), other);
        return *this;
    }
    inline ThreeVector& operator+= (B& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator+=), other);
        return *this;
    }
    inline ThreeVector& operator+= (const T& other)
    {
        lift (std::mem_fn<B&,B,const T&> (&B::operator+=), other);
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (ThreeVector<U>& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator*=), other);
        return *this;
    }
    inline ThreeVector& operator*= (B& other)
    {
        lift (std::mem_fn<B&,B,const B&> (&B::operator*=), other);
        return *this;
    }
    inline ThreeVector& operator*= (const T& other)
    {
        lift (std::mem_fn<B&,B,const T&> (&B::operator*=), other);
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
