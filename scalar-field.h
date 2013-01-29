//
//  scalar-field.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_scalar_field_h
#define vf_pic_scalar_field_h

#include "global.h"

#include <iostream>

/* Declerations */

template <typename T, int N1, int N2>
class ScalarBase
{
protected:
    ScalarBase (T*);
    ScalarBase (const ScalarBase&) = delete;
    ScalarBase (ScalarBase&&) noexcept;
    ScalarBase& operator= (ScalarBase&&) = delete;
    virtual ~ScalarBase () noexcept;
public:
    ScalarBase& operator= (const ScalarBase&);
    ScalarBase& operator= (const T&);
    inline T& operator() (int i1, int i2)
    {
        return data[i1*(N2 + 2) + i2];
    }
    inline const T& operator() (int i1, int i2) const
    {
        return data[i1*(N2 + 2) + i2];
    }
    inline friend std::ostream& operator<< (std::ostream& os, const ScalarBase& scalar)
    {
        const auto buffer = reinterpret_cast<char *> (scalar.data);
        os.write (buffer, size*sizeof (T));
        return os;
    }
protected:
    T *data;
    static const int size = (N1 + 2)*(N2 + 2);
public:
    static const int n1 = N1;
    static const int n2 = N2;
};

template <typename T, int N1, int N2>
class ScalarField: public ScalarBase<T,N1,N2>
{
public:
    ScalarField ();
    ScalarField (const ScalarField&);
    ScalarField& operator= (const ScalarField&);
    ScalarField (ScalarField&&) noexcept;
    ScalarField& operator= (ScalarField&&) = delete;
    virtual ~ScalarField () noexcept;
    using ScalarBase<T,N1,N2>::operator=;
    using ScalarBase<T,N1,N2>::data;
    using ScalarBase<T,N1,N2>::size;
};

template <typename T>
using GlobalScalarField = ScalarField<T,global::nz,global::nx>;

template <typename T>
using LocalScalarField = ScalarField<T,global::mz,global::mx>;

template <typename T>
class LocalScalarFieldView: public ScalarBase<T,global::mz,global::mx>
{
public:
    LocalScalarFieldView (GlobalScalarField<T>&, int);
    using ScalarBase<T,global::mz,global::mx>::operator=;
};

/* Implementation of ScalarBase */

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::ScalarBase (T *ptr): data (ptr)
{
    std::cout << "ScalarBase (" << this << "): Pointer ctor\n";
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator= (const ScalarBase& other)
{
    std::cout << "ScalarBase (" << this << "): Copy assign\n";
    if (this != &other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) = other(i1,i2);
        }
    }
    return *this;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::ScalarBase (ScalarBase&& other) noexcept: data (other.data)
{
    std::cout << "ScalarBase (" << &other << ", " << this << "): Move ctor\n";
    other.data = nullptr;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::~ScalarBase () noexcept
{
    std::cout << "ScalarBase (" << this << "): Destructor\n";
    data = nullptr;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator= (const T& value)
{
    std::cout << "ScalarBase (" << this << "): Copy assign\n";
    for (int i1 = 1; i1 <= N1; ++i1)
    for (int i2 = 1; i2 <= N2; ++i2)
    {
        (*this)(i1,i2) = value;
    }
    return *this;
}

/* Implementation of ScalarField */

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (): ScalarBase<T,N1,N2> (new T[size])
{
    std::cout << "ScalarField (" << this << "): Default ctor (Allocated memory)\n";
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (const ScalarField& other): ScalarBase<T,N1,N2> (new T[size])
{
    std::cout << "ScalarField (" << this << "): Copy ctor (Allocated memory)\n";
    ScalarBase<T,N1,N2>::operator= (other);
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>& ScalarField<T,N1,N2>::operator= (const ScalarField& other)
{
    std::cout << "ScalarField (" << this << "): Copy assign\n";
    ScalarBase<T,N1,N2>::operator= (other);
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (ScalarField&& other) noexcept: ScalarBase<T,N1,N2> (std::move (other))
{
    std::cout << "ScalarField (" << this << "): Move ctor\n";
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::~ScalarField () noexcept
{
    std::cout << "ScalarField (" << this << "): Destructor";
    if (data != nullptr)
    {
        std::cout << " (Deallocating memory...)";
    }
    std::cout << std::endl;
    delete[] data;
}

/* Implementation of LocalScalarFieldView */

template <typename T>
LocalScalarFieldView<T>::LocalScalarFieldView
(GlobalScalarField<T>& global, int ithread): ScalarBase<T,global::mz,global::mx> (&global(ithread*global::mz,0))
{
    std::cout << "LocalScalarFieldView (" << this << "): Global ctor\n";
}

#endif
