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

template <typename T, uint Nz, uint Nx>
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
    inline T& operator() (uint k, uint i)
    {
#ifdef DEBUG
        assert (0 <= k && k < Nz + 2*nghost && 0 <= i && i < Nx + 2*nghost);
#endif
        return data[k*stride + i];
    }
    inline const T& operator() (uint k, uint i) const
    {
#ifdef DEBUG
        assert (0 <= k && k < Nz + 2*nghost && 0 <= i && i < Nx + 2*nghost);
#endif
        return data[k*stride + i];
    }
    /* Assign */
    inline ScalarField& operator= (const ScalarField& other)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) = other(k,i);
        }
        return *this;
    }
    inline ScalarField& operator= (const T& value)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) = value;
        }
        return *this;
    }
    /* Add to */
    inline ScalarField& operator+= (const ScalarField& other)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) += other(k,i);
        }
        return *this;
    }
    inline ScalarField& operator+= (const T& value)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) += value;
        }
        return *this;
    }
    /* Subtract from */
    inline ScalarField& operator-= (const ScalarField& other)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) -= other(k,i);
        }
        return *this;
    }
    inline ScalarField& operator-= (const T& value)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) -= value;
        }
        return *this;
    }
    /* Multiply with */
    inline ScalarField& operator*= (const ScalarField& other)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) *= other(k,i);
        }
        return *this;
    }
    inline ScalarField& operator*= (const T& value)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) *= value;
        }
        return *this;
    }
    /* Divide by */
    inline ScalarField& operator/= (const ScalarField& other)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) /= other(k,i);
        }
        return *this;
    }
    inline ScalarField& operator/= (const T& value)
    {
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            (*this)(k,i) /= value;
        }
        return *this;
    }
    void fill (const T& value)
    {
        std::fill (data, data + size, value);
    }
    real variance () const
    {
        real sum = 0.0;
        for (uint k = k1; k < k2; ++k)
#ifdef __INTEL_COMPILER
#pragma vector aligned
#pragma ivdep
#endif
        for (uint i = i1; i < i2; ++i)
        {
            real element = (*this) (k,i);
            sum += element*element;
        }
        return sum/real (Nz*Nx);
    }
    real rms () const
    {
        return sqrt (this->variance ());
    }

protected:
    T *data;
public:
    static const uint nghost = vfpic::nghost;
    static const uint i1 = nghost, i2 = i1 + Nx;
    static const uint k1 = nghost, k2 = k1 + Nz;
protected:
    static const uint stride = (Nx + 2*nghost);
    static const uint size = stride*(Nz + 2*nghost);
};

template <typename T, uint Nz, uint Nx>
struct NewScalarField: public ScalarField<T,Nz,Nx>
{
    NewScalarField (): ScalarField<T,Nz,Nx>
#ifdef __INTEL_COMPILER
    (reinterpret_cast<T*> (_mm_malloc (size*sizeof (T), vfpic::alignment)))
#else
    (new T[size])
#endif
    {
        if (config::verbose)
        {
            printf ("NewScalarField (Default ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
    }
    NewScalarField (const NewScalarField& other): ScalarField<T,Nz,Nx>
#ifdef __INTEL_COMPILER
    (reinterpret_cast<T*> (_mm_malloc (size*sizeof (T), vfpic::alignment)))
#else
    (new T[size])
#endif
    {
        if (config::verbose)
        {
            printf ("NewScalarField (Copy ctor): Allocated %lu bytes.\n", size*sizeof (T));
        }
        ScalarField<T,Nz,Nx>::operator= (other);
    }
    NewScalarField& operator= (const NewScalarField&) = default;
    NewScalarField (NewScalarField&& other) noexcept: ScalarField<T,Nz,Nx> (other.data)
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
#ifdef __INTEL_COMPILER
        _mm_free (data);
#else
        delete[] data;
#endif
    }
    
    using ScalarField<T,Nz,Nx>::operator=;

    using ScalarField<T,Nz,Nx>::data;
    using ScalarField<T,Nz,Nx>::size;
    
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

template <typename T, uint Nz, uint Nx>
struct ScalarFieldView: public ScalarField<T,Nz,Nx>
{
    template <uint Mz, uint Mx>
    ScalarFieldView (ScalarField<T,Mz,Mx>& global, const uint ithread):
    ScalarField<T,Nz,Nx> (&global(ithread*Nz,0))
    {
        using vfpic::nthreads;
        static_assert (Mz == nthreads*Nz && Mx == Nx, "");
    }
    template <uint Mz, uint Mx>
    ScalarFieldView (const ScalarField<T,Mz,Mx>& global, const uint ithread):
    ScalarField<T,Nz,Nx> (const_cast<T*> (&global(ithread*Nz,0)))
    {
        static_assert (Mz == vfpic::nthreads*Nz && Mx == Nx, "");
    }
    using ScalarField<T,Nz,Nx>::operator=;
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
