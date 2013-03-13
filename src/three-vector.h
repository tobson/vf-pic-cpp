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
    inline T& operator[] (uint j)
    {
        return array[j];
    }
    inline const T& operator[] (uint j) const
    {
        return array[j];
    }
    inline T& at (uint j)
    {
        return array.at (j);
    }
    inline const T& at (uint j) const
    {
        return array.at (j);
    }
    friend std::ostream& operator<< (std::ostream& os, const Vector vec)
    {
        os.write (reinterpret_cast<char*> (&vec.x), 3*sizeof (T));
        return os;
    }
    friend std::istream& operator>> (std::istream& is, Vector vec)
    {
        is.read (reinterpret_cast<char*> (&vec.x), 3*sizeof (T));
        return is;
    }
};

#endif
