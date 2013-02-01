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
    inline ParticleBase& operator= (const ParticleBase& other)
    {
        if (this != other)
        {
            for (int index = 0; index < N; ++index)
            {
                particles[index] = other.particles[index];
            }
        }
    }
    inline ParticleBase& operator= (const T& other)
    {
        if (this != other)
        {
            for (int index = 0; index < N; ++index)
            {
                particles[index] = other;
            }
        }
    }
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
}

template <typename T, int N>
ParticleBase<T,N>::ParticleBase (ParticleBase&& other) noexcept:
particles (other.particles)
{
    other.particles = nullptr;
}

template <typename T, int N>
ParticleBase<T,N>::~ParticleBase () noexcept
{
    particles = nullptr;
}

/* Implementation of ParticleArray */

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (): ParticleBase<T,N> (new Particle<T>[N])
{
    if (config::verbose)
    {
        printf ("ParticleArray (Default ctor): Allocated %lu bytes.\n", N*sizeof (T));
    }
}

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (const ParticleArray& other): ParticleBase<T,N> (new Particle<T>[N])
{
    if (config::verbose)
    {
        printf ("ParticleArray (Copy ctor): Allocated %lu bytes.\n", N*sizeof (T));
    }
    ParticleBase<T,N>::operator= (other);
}

template <typename T, int N>
ParticleArray<T,N>& ParticleArray<T,N>::operator= (const ParticleArray& other)
{
    ParticleBase<T,N>::operator= (other);
}

template <typename T, int N>
ParticleArray<T,N>::ParticleArray (ParticleArray&& other) noexcept:
ParticleBase<T,N> (std::move (other))
{
}

template <typename T, int N>
ParticleArray<T,N>::~ParticleArray () noexcept
{
    if (config::verbose)
    {
         if (this->particles != nullptr)
        {
            printf ("ParticleArray: Deallocated %lu bytes.\n", N*sizeof (T));
        }
    }
    delete[] this->particles;
}

/* Implementations of LocalParticleArrayView */

template <typename T>
LocalParticleArrayView<T>::LocalParticleArrayView (GlobalParticleArray<T>& array, int ithread):
ParticleBase<T, vfpic::mpar>(&array[ithread*vfpic::mpar])
{
}

#endif /* defined(__vf_pic__particle__) */
