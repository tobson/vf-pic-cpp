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
union Vector
{
private:
    std::array<T,3> vec;
public:
    struct
    {
        T x, y, z;
    };
    inline T& operator[] (size_t j)
    {
        return vec[j];
    }
    inline const T& operator[] (size_t j) const
    {
        return vec[j];
    }
    inline T& at (size_t j)
    {
        return vec.at (j);
    }
    inline const T& at (size_t j) const
    {
        return vec.at (j);
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
