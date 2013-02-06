//
//  scalar-field.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_scalar_field_h
#define vf_pic_scalar_field_h

#include "config.h"
#include "global.h"

#include <algorithm>
#include <iostream>
#include <cstdio>

/* Declerations */

template <typename T, int N1, int N2>
class ScalarBase
{
protected:
    explicit ScalarBase (T *ptr): data (ptr)
    {
    }
public:
    /* Index operators */
    inline T& operator() (int i1, int i2)
    {
        return data[i1*(N2 + 2) + i2];
    }
    inline const T& operator() (int i1, int i2) const
    {
        return data[i1*(N2 + 2) + i2];
    }
    /* Assign */
    inline ScalarBase& operator= (const ScalarBase& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = other(i1,i2);
        }
        return *this;
    }
    inline ScalarBase& operator= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = value;
        }
        return *this;
    }
    /* Add to */
    inline ScalarBase& operator+= (const ScalarBase& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += other(i1,i2);
        }
        return *this;
    }
    inline ScalarBase& operator+= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += value;
        }
        return *this;
    }
    /* Subtract from */
    inline ScalarBase& operator-= (const ScalarBase& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) -= other(i1,i2);
        }
        return *this;
    }
    inline ScalarBase& operator-= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) -= value;
        }
        return *this;
    }
    /* Multiply with */
    inline ScalarBase& operator*= (const ScalarBase& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) *= other(i1,i2);
        }
        return *this;
    }
    inline ScalarBase& operator*= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) *= value;
        }
        return *this;
    }
    /* Divide by */
    inline ScalarBase& operator/= (const ScalarBase& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) /= other(i1,i2);
        }
        return *this;
    }
    inline ScalarBase& operator/= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) /= value;
        }
        return *this;
    }
    void reciprocal ()
    {
        const real one = T (1);
        
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = one/(*this)(i1,i2);
        }
    }
protected:
    T *data;
    static const unsigned long size = (N1 + 2)*(N2 + 2);
};

template <typename T, int N1, int N2>
struct ScalarField: public ScalarBase<T,N1,N2>
{
    ScalarField (): ScalarBase<T,N1,N2> (new T[size])
    {
        if (config::verbose)
        {
            printf ("ScalarField (Default ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
    }
    ScalarField (const ScalarField& other): ScalarBase<T,N1,N2> (new T[size])
    {
        if (config::verbose)
        {
            printf ("ScalarField (Copy ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
        ScalarBase<T,N1,N2>::operator= (other);
    }
    ScalarField& operator= (const ScalarField&) = default;
    ScalarField (ScalarField&& other) noexcept: ScalarBase<T,N1,N2> (other.data)
    {
        other.data = nullptr;
    }
    ScalarField& operator= (ScalarField&&) = delete;
    ~ScalarField () noexcept
    {
        if (config::verbose)
        {
            if (data != nullptr)
            {
                printf ("ScalarField: Deallocating %lu bytes...\n", size*sizeof (T));
            }
        }
        delete[] data;
    }
    
    using ScalarBase<T,N1,N2>::operator=;
    using ScalarBase<T,N1,N2>::operator+=;
    using ScalarBase<T,N1,N2>::operator-=;
    using ScalarBase<T,N1,N2>::operator*=;
    using ScalarBase<T,N1,N2>::operator/=;
    using ScalarBase<T,N1,N2>::data;
    using ScalarBase<T,N1,N2>::size;
    
    void fill (const T& value)
    {
        std::fill (data, data + size, value);
    }
    friend std::ostream& operator<< (std::ostream& os, const ScalarField& scalar)
    {
        os.write (reinterpret_cast<char *> (scalar.data), size*sizeof (T));
        return os;
    }
};

template <typename T>
using GlobalScalarField = ScalarField<T,vfpic::nz,vfpic::nx>;

template <typename T>
using LocalScalarField = ScalarField<T,vfpic::mz,vfpic::mx>;

template <typename T, int N1, int N2>
struct ScalarBaseView: public ScalarBase<T,N1,N2>
{
    ScalarBaseView (GlobalScalarField<T>& global, int ithread):
    ScalarBase<T,N1,N2> (&global(ithread*N1,0))
    {
        static_assert (N1 == vfpic::mz && N2 == vfpic::mx, "");
    }
    using ScalarBase<T,N1,N2>::operator=;
    using ScalarBase<T,N1,N2>::operator+=;
    using ScalarBase<T,N1,N2>::operator-=;
    using ScalarBase<T,N1,N2>::operator*=;
    using ScalarBase<T,N1,N2>::operator/=;
};

template <typename T>
using LocalScalarFieldView = ScalarBaseView<T,vfpic::mz,vfpic::mx>;

#endif
