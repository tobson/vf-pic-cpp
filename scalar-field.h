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
    ScalarBase& operator+= (const ScalarBase&);
    ScalarBase& operator+= (const T&);
    ScalarBase& operator*= (const ScalarBase&);
    ScalarBase& operator*= (const T&);
    inline T& operator() (int i1, int i2)
    {
        return data[i1*(N2 + 2) + i2];
    }
    inline const T& operator() (int i1, int i2) const
    {
        return data[i1*(N2 + 2) + i2];
    }
protected:
    T *data;
    static const int size = (N1 + 2)*(N2 + 2);
public:
    static const int nz = N1;
    static const int nx = N2;
    typedef T value_type;
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
    friend std::ostream& operator<< (std::ostream& os, const ScalarField& scalar)
    {
        const auto buffer = reinterpret_cast<char *> (scalar.data);
        os.write (buffer, size*sizeof (T));
        return os;
    }
};

template <typename T>
struct GlobalScalarField: public ScalarField<T,vfpic::nz,vfpic::nx>
{
    using ScalarBase<T,vfpic::nz,vfpic::nx>::operator=;
    using ScalarBase<T,vfpic::nz,vfpic::nx>::operator+=;
    using ScalarBase<T,vfpic::nz,vfpic::nx>::operator*=;
};

template <typename T>
struct LocalScalarField: public ScalarField<T,vfpic::mz,vfpic::mx>
{
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator+=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator*=;
};

template <typename T>
struct LocalScalarFieldView: public ScalarBase<T,vfpic::mz,vfpic::mx>
{
    LocalScalarFieldView (GlobalScalarField<T>&, int);
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator+=;
    using ScalarBase<T,vfpic::mz,vfpic::mx>::operator*=;
};

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

/* Implementation of ScalarBase */

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::ScalarBase (T *ptr): data (ptr)
{
    if (config::verbose)
    {
        std::cout << "ScalarBase (" << this << "): Pointer ctor\n";
    }
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::ScalarBase (ScalarBase&& other) noexcept: data (other.data)
{
    if (config::verbose)
    {
        std::cout << "ScalarBase (" << &other << ", " << this << "): Move ctor\n";
    }
    other.data = nullptr;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>::~ScalarBase () noexcept
{
    if (config::verbose)
    {
        std::cout << "ScalarBase (" << this << "): Destructor\n";
    }
    data = nullptr;
}
        
template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator= (const ScalarBase& other)
{
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
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator= (const T& value)
{
    for (int i1 = 1; i1 <= N1; ++i1)
    for (int i2 = 1; i2 <= N2; ++i2)
    {
        (*this)(i1,i2) = value;
    }
    return *this;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator+= (const ScalarBase& other)
{
    if (this != &other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += other(i1,i2);
        }
    }
    return *this;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator+= (const T& value)
{
    for (int i1 = 1; i1 <= N1; ++i1)
    for (int i2 = 1; i2 <= N2; ++i2)
    {
        (*this)(i1,i2) += value;
    }
    return *this;
}
        
template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator*= (const ScalarBase& other)
{
    if (this != &other)
    {
        for (int i1 = 1; i1 <= N1; ++i1)
        for (int i2 = 1; i2 <= N2; ++i2)
        {
            (*this)(i1,i2) += other(i1,i2);
        }
    }
    return *this;
}

template <typename T, int N1, int N2>
ScalarBase<T,N1,N2>& ScalarBase<T,N1,N2>::operator*= (const T& value)
{
    for (int i1 = 1; i1 <= N1; ++i1)
    for (int i2 = 1; i2 <= N2; ++i2)
    {
        (*this)(i1,i2) += value;
    }
    return *this;
}
        
/* Implementation of ScalarField */

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (): ScalarBase<T,N1,N2> (new T[size])
{
    if (config::verbose)
    {
        std::cout << "ScalarField (" << this << "): Default ctor (Allocated memory)\n";
    }
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (const ScalarField& other): ScalarBase<T,N1,N2> (new T[size])
{
    if (config::verbose)
    {
        std::cout << "ScalarField (" << this << "): Copy ctor (Allocated memory)\n";
    }
    ScalarBase<T,N1,N2>::operator= (other);
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>& ScalarField<T,N1,N2>::operator= (const ScalarField& other)
{
    if (config::verbose)
    {
        std::cout << "ScalarField (" << this << "): Copy assign\n";
    }
    ScalarBase<T,N1,N2>::operator= (other);
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::ScalarField (ScalarField&& other) noexcept: ScalarBase<T,N1,N2> (std::move (other))
{
    if (config::verbose)
    {
        std::cout << "ScalarField (" << this << "): Move ctor\n";
    }
}

template <typename T, int N1, int N2>
ScalarField<T,N1,N2>::~ScalarField () noexcept
{
    if (config::verbose)
    {
        std::cout << "ScalarField (" << this << "): Destructor";
        if (data != nullptr)
        {
            std::cout << " (Deallocating memory...)";
        }
        std::cout << std::endl;
    }
    delete[] data;
}

/* Implementation of LocalScalarFieldView */

template <typename T>
LocalScalarFieldView<T>::LocalScalarFieldView
(GlobalScalarField<T>& global, int ithread): ScalarBase<T,vfpic::mz,vfpic::mx> (&global(ithread*vfpic::mz,0))
{
    if (config::verbose)
    {
        std::cout << "LocalScalarFieldView (" << this << "): Global ctor\n";
    }
}

#endif
