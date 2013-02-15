//
//  three-vector.h
//  vf-pic
//
//  Created by Tobias Heinemann on 2/14/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_h
#define vf_pic_three_vector_h

#include <array>
#include <iostream>

template <typename T>
struct Vector
{
private:
    std::array<T,3> array;
public:
    T& x; T& y; T& z;
    Vector (): x (array.at (0)), y (array.at (1)), z (array.at (2)) {}
    inline T& operator[] (size_t j)
    {
        return array[j];
    }
    inline const T& operator[] (size_t j) const
    {
        return array[j];
    }
    inline T& at (size_t j)
    {
        return array.at (j);
    }
    inline const T& at (size_t j) const
    {
        return array.at (j);
    }
    friend std::ostream& operator<< (std::ostream& os, Vector vec)
    {
        os << vec.x;
        os << vec.y;
        os << vec.z;
        return os;
    }
};

#endif
