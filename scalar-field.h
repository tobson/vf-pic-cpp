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
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = T (1)/(*this)(i1,i2);
        }
    }
protected:
    T *data;
public:
    static const int nz = N1;
    static const int nx = N2;
    static const unsigned long size = (nx + 2)*(nz + 2);
    typedef T value_type;
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
    
    typedef ScalarBase<T,N1,N2> base_type;
    
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
    ScalarBaseView
    (GlobalScalarField<T>& global, int ithread): ScalarBase<T,N1,N2> (&global(ithread*vfpic::mz,0))
    {
        static_assert (N1 == vfpic::mz && N2 == vfpic::mx, "");
    }
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator+=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator-=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator*=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator/=;
    
    typedef ScalarBase<T,N1,N2> base_type;
};

template <typename T>
using LocalScalarFieldView = ScalarBaseView<T,vfpic::mz,vfpic::mx>;

/* Traits */

template <class S>
struct is_scalarfield
{
    static const bool value = false;
};

template <typename T, int N1, int N2>
struct is_scalarfield<ScalarBase<T,N1,N2>>
{
    static const bool value = true;
};
template <typename T, int N1, int N2>
struct is_scalarfield<ScalarField<T,N1,N2>>
{
    static const bool value = true;
};
template <typename T>
struct is_scalarfield<GlobalScalarField<T>>
{
    static const bool value = true;
};
template <typename T>
struct is_scalarfield<LocalScalarField<T>>
{
    static const bool value = true;
};
template <typename T>
struct is_scalarfield<LocalScalarFieldView<T>>
{
    static const bool value = true;
};

#endif
