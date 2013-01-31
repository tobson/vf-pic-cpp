//
//  particle.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__particle__
#define __vf_pic__particle__

#include "config.h"
#include "global.h"

#include <iostream>

/* Declerations */

template <typename T>
struct Particle
{
    T x, y, z;
    T vx, vy, vz;
    T ex, ey, ez;
    T bx, by, bz;
};

template <typename T, int N>
class ParticleBase
{
protected:
    ParticleBase (Particle<T>*);
    ParticleBase (const ParticleBase&) = delete;
    ParticleBase (ParticleBase&&) noexcept;
    ParticleBase& operator= (ParticleBase&&) = delete;
    virtual ~ParticleBase () noexcept;
public:
    ParticleBase& operator= (const ParticleBase&);
    inline Particle<T>& operator[] (int index)
    {
        return particles[index];
    }
    inline const Particle<T>& operator[] (int index) const
    {
        return particles[index];
    }
protected:
    Particle<T> *particles;
};

template <typename T, int N>
class ParticleArray: public ParticleBase<T,N>
{
public:
    ParticleArray ();
    ParticleArray (const ParticleArray&);
    ParticleArray& operator= (const ParticleArray&);
    ParticleArray (ParticleArray&&) noexcept;
    ParticleArray& operator= (ParticleArray&&) = delete;
    virtual ~ParticleArray () noexcept;
    using ParticleBase<T,N>::operator=;
};

template <typename T>
class GlobalParticleArray: public ParticleArray<T,vfpic::npar>
{
};

template <typename T>
struct LocalParticleArrayView: public ParticleBase<T,vfpic::mpar>
{
    LocalParticleArrayView (GlobalParticleArray<T>&, int);
};

/* Implementation of ParticleBase */

template <typename T, int N>
ParticleBase<T,N>::ParticleBase (Particle<T> *ptr): particles (ptr)
{
    if (config::verbose)
    {
        std::cout << "ParticleBase (" << this << "): Pointer ctor\n";
    }
}

template <typename T, int N>
ParticleBase<T,N>& ParticleBase<T,N>::operator= (const ParticleBase<T,N>& other)
{
    if (config::verbose)
    {
        std::cout << "ParticleBase (" << this << "): Copy assign\n";
    }
    if (this != other)
    {
        for (int index = 0; index < N; ++index)
        {
            (*this)[index] = other[index];
        }
    }
}

template <typename T, int N>
ParticleBase<T,N>::ParticleBase (ParticleBase&& other) noexcept:
particles (other.particles)
{
    if (config::verbose)
    {
        std::cout << "ParticleBase (" << this << "): Move ctor\n";
    }
    other.particles = nullptr;
}

template <typename T, int N>
ParticleBase<T,N>::~ParticleBase () noexcept
{
    if (config::verbose)
    {
        std::cout << "ParticleBase (" << this << "): Destructor\n";
    }
    particles = nullptr;
}

/* Implementation of ParticleArray */

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (): ParticleBase<T,N> (new Particle<T>[N])
{
    if (config::verbose)
    {
        std::cout << "ParticleArray (" << this << "): Default ctor (Allocated memory)\n";
    }
}

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (const ParticleArray& other):
ParticleBase<T,N> (new Particle<T>[N])
{
    if (config::verbose)
    {
        std::cout << "ParticleArray (" << this << "): Copy ctor (Allocated memory)\n";
    }
    ParticleBase<T,N>::operator= (other);
}

template <typename T, int N>
ParticleArray<T,N>& ParticleArray<T,N>::operator= (const ParticleArray& other)
{
    if (config::verbose)
    {
        std::cout << "ParticleArray (" << this << "): Copy assign\n";
    }
    ParticleBase<T,N>::operator= (other);
}

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (ParticleArray&& other) noexcept:
ParticleBase<T,N> (std::move (other))
{
    if (config::verbose)
    {
        std::cout << "ParticleArray (" << this << "): Move ctor\n";
    }
}

template <typename T, int N>
ParticleArray<T,N>::~ParticleArray () noexcept
{
    if (config::verbose)
    {
        std::cout << "ParticleArray (" << this << "): Destructor";
        if (this->particles != nullptr)
        {
            std::cout << " (Deallocating memory...)";
        }
        std::cout << std::endl;
    }
    delete[] this->particles;
}

/* Implementations of LocalParticleArrayView */

template <typename T>
LocalParticleArrayView<T>::LocalParticleArrayView (GlobalParticleArray<T>& array, int ithread):
ParticleBase<T, vfpic::mpar>(&array[ithread*vfpic::mpar])
{
    if (config::verbose)
    {
        std::cout << "LocalParticleArrayView (" << this << "): Global ctor\n";
    }
}

#endif /* defined(__vf_pic__particle__) */
