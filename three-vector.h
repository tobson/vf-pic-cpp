//
//  three-vector.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_three_vector_h
#define vf_pic_three_vector_h

/* Class decleration */

template <class S>
class ThreeVector
{
public:
  ThreeVector ();
  template <class U>
  ThreeVector& operator= (ThreeVector<U>& other)
  {
    std::cout << "ThreeVector (" << this << "): Copy assign\n";
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
  }
  ThreeVector (const S&, const S&, const S&);
  ThreeVector (S&&, S&&, S&&);
  S& operator[] (int);
  const S& operator[] (int) const;
  S x, y, z;
};

/* Class implementation */

template <class S>
ThreeVector<S>::ThreeVector ():
x (), y (), z ()
{
}

template <class S>
ThreeVector<S>::ThreeVector (const S& a, const S& b, const S& c):
x (a), y (b), z (c)
{
}

template <class S>
ThreeVector<S>::ThreeVector (S&& a, S&& b, S&& c):
x (std::move (a)), y (std::move (b)), z (std::move (c))
{
}

template <class S>
S& ThreeVector<S>::operator[] (int j)
{
  switch (j)
  {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    default: throw std::range_error ("Index out of bounds");
  }
}

template <class S>
const S& ThreeVector<S>::operator[] (int j) const
{
  switch (j)
  {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    default: throw std::range_error ("Index out of bounds");
  }
}

#endif
