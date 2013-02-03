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
    T x, z;
    T vx, vy, vz;
};

template <typename T, int N>
class ParticleBase
{
protected:
    explicit ParticleBase (Particle<T> *ptr): particles (ptr)
    {
    }
public:
    inline ParticleBase& operator= (const ParticleBase& other)
    {
        std::copy (other.particles, other.particles + N, particles);
    }
    inline ParticleBase& operator= (const T& value)
    {
        std::fill (particles, particles + N, value);
    }
    inline Particle<T>& operator[] (int index)
    {
        return particles[index];
    }
    inline const Particle<T>& operator[] (int index) const
    {
        return particles[index];
    }
    Particle<T> *begin()
    {
        return particles;
    }
    Particle<T> *end()
    {
        return particles + N;
    }
    const Particle<T> *begin() const
    {
        return particles;
    }
    const Particle<T> *end() const
    {
        return particles + N;
    }
protected:
    Particle<T> *particles;
};

template <typename T, int N>
class ParticleArray: public ParticleBase<T,N>
{
public:
    ParticleArray (): ParticleBase<T,N> (new Particle<T>[N])
    {
        if (config::verbose)
        {
            printf ("ParticleArray (Default ctor): Allocated %lu bytes.\n", N*sizeof (T));
        }
    }
    ParticleArray (const ParticleArray& other): ParticleBase<T,N> (new Particle<T>[N])
    {
        if (config::verbose)
        {
            printf ("ParticleArray (Copy ctor): Allocated %lu bytes.\n", N*sizeof (T));
        }
        ParticleBase<T,N>::operator= (other);
    }
    ParticleArray& operator= (const ParticleArray&) = default;
    ParticleArray (ParticleArray&& other) noexcept: ParticleBase<T,N> (other.particles)
    {
        other.particles = nullptr;
    }
    ParticleArray& operator= (ParticleArray&&) = delete;
    ~ParticleArray () noexcept
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
    using ParticleBase<T,N>::operator=;
};

template <typename T>
using GlobalParticleArray = ParticleArray<T,vfpic::npar>;

template <typename T>
struct LocalParticleArrayView: public ParticleBase<T,vfpic::mpar>
{
    LocalParticleArrayView (GlobalParticleArray<T>& array, int ithread):
    ParticleBase<T, vfpic::mpar>(&array[ithread*vfpic::mpar])
    {
    }
};

#endif /* defined(__vf_pic__particle__) */
