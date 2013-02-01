//
//  three-vector.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_h
#define vf_pic_three_vector_h

#include <iostream>
#include <stdexcept>

/* Class decleration */

template <class S>
class ThreeVector
{
public:
    ThreeVector ()
    {
    }
    ThreeVector (const S& a, const S& b, const S& c): x (a), y (b), z (c)
    {
    }
    ThreeVector (S&& a, S&& b, S&& c): x (std::move (a)), y (std::move (b)), z (std::move (c))
    {
    }
    template <class U>
    inline ThreeVector& operator= (ThreeVector<U>& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    inline ThreeVector& operator= (S& other)
    {
        x = other;
        y = other;
        z = other;
        return *this;
    }
    inline ThreeVector& operator= (const typename S::value_type other)
    {
        x = other;
        y = other;
        z = other;
        return *this;
    }
    template <class U>
    inline ThreeVector& operator+= (ThreeVector<U>& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    inline ThreeVector& operator+= (S& other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }
    inline ThreeVector& operator+= (const typename S::value_type other)
    {
        x += other;
        y += other;
        z += other;
        return *this;
    }
    template <class U>
    inline ThreeVector& operator*= (ThreeVector<U>& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    inline ThreeVector& operator*= (S& other)
    {
        x *= other;
        y *= other;
        z *= other;
        return *this;
    }
    inline ThreeVector& operator*= (const typename S::value_type other)
    {
        x *= other;
        y *= other;
        z *= other;
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
