//
//  scalar-field.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_scalar_field_h
#define vf_pic_scalar_field_h

#include <algorithm>
#include <cstdio>
#ifdef DEBUG
#include <cassert>
#endif
#include <iostream>

#include "config.h"
#include "global.h"

/* Declerations */

template <typename T, int N1, int N2>
class ScalarField
{
protected:
    explicit ScalarField (T *ptr): data (ptr)
    {
    }
    ScalarField (const ScalarField&) = default;
public:
    virtual ~ScalarField () noexcept = default;
    /* Index operators */
    inline T& operator() (int i1, int i2)
    {
#ifdef DEBUG
        assert (0 <= i1 < N1+2 && 0 <= i2 < N2+2);
#endif
        return data[i1*(N2 + 2) + i2];
    }
    inline const T& operator() (int i1, int i2) const
    {
#ifdef DEBUG
        assert (0 <= i1 < N1+2 && 0 <= i2 < N2+2);
#endif
        return data[i1*(N2 + 2) + i2];
    }
    /* Assign */
    inline ScalarField& operator= (const ScalarField& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = other(i1,i2);
        }
        return *this;
    }
    inline ScalarField& operator= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = value;
        }
        return *this;
    }
    /* Add to */
    inline ScalarField& operator+= (const ScalarField& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += other(i1,i2);
        }
        return *this;
    }
    inline ScalarField& operator+= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += value;
        }
        return *this;
    }
    /* Subtract from */
    inline ScalarField& operator-= (const ScalarField& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) -= other(i1,i2);
        }
        return *this;
    }
    inline ScalarField& operator-= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) -= value;
        }
        return *this;
    }
    /* Multiply with */
    inline ScalarField& operator*= (const ScalarField& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) *= other(i1,i2);
        }
        return *this;
    }
    inline ScalarField& operator*= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) *= value;
        }
        return *this;
    }
    /* Divide by */
    inline ScalarField& operator/= (const ScalarField& other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) /= other(i1,i2);
        }
        return *this;
    }
    inline ScalarField& operator/= (const T& value)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) /= value;
        }
        return *this;
    }
    void fill (const T& value)
    {
        std::fill (data, data + size, value);
    }
protected:
    T *data;
    static const unsigned long size = (N1 + 2)*(N2 + 2);
};

template <typename T, int N1, int N2>
struct NewScalarField: public ScalarField<T,N1,N2>
{
    NewScalarField (): ScalarField<T,N1,N2> (new T[size])
    {
        if (config::verbose)
        {
            printf ("NewScalarField (Default ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
    }
    NewScalarField (const NewScalarField& other): ScalarField<T,N1,N2> (new T[size])
    {
        if (config::verbose)
        {
            printf ("NewScalarField (Copy ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
        ScalarField<T,N1,N2>::operator= (other);
    }
    NewScalarField& operator= (const NewScalarField&) = default;
    NewScalarField (NewScalarField&& other) noexcept: ScalarField<T,N1,N2> (other.data)
    {
        other.data = nullptr;
    }
    NewScalarField& operator= (NewScalarField&&) = delete;
    ~NewScalarField () noexcept
    {
        if (config::verbose)
        {
            if (data != nullptr)
            {
                printf ("NewScalarField: Deallocating %lu bytes...\n", size*sizeof (T));
            }
        }
        delete[] data;
    }
    
    using ScalarField<T,N1,N2>::operator=;

    using ScalarField<T,N1,N2>::data;
    using ScalarField<T,N1,N2>::size;
    
    friend std::ostream& operator<< (std::ostream& os, const NewScalarField& scalar)
    {
        os.write (reinterpret_cast<char *> (scalar.data), size*sizeof (T));
        return os;
    }
    friend std::istream& operator>> (std::istream& is, NewScalarField& scalar)
    {
        is.read (reinterpret_cast<char *> (scalar.data), size*sizeof (T));
        return is;
    }
};

template <typename T, int N1, int N2>
struct ScalarFieldView: public ScalarField<T,N1,N2>
{
    template <int M1, int M2>
    ScalarFieldView (ScalarField<T,M1,M2>& global, const int ithread):
    ScalarField<T,N1,N2> (&global(ithread*N1,0))
    {
        using vfpic::nthreads;
        static_assert (M1 == nthreads*N1 && M2 == N2, "");
    }
    using ScalarField<T,N1,N2>::operator=;
};

template <typename T = real>
using GlobalScalarField = ScalarField<T,vfpic::nz,vfpic::nx>;

template <typename T = real>
using LocalScalarField = ScalarField<T,vfpic::mz,vfpic::mx>;

template <typename T = real>
using NewGlobalScalarField = NewScalarField<T,vfpic::nz,vfpic::nx>;

template <typename T = real>
using NewLocalScalarField = NewScalarField<T,vfpic::mz,vfpic::mx>;

template <typename T = real>
using LocalScalarFieldView = ScalarFieldView<T,vfpic::mz,vfpic::mx>;

#endif