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
class Particles
{
protected:
    explicit Particles (Particle<T> *ptr): array (ptr)
    {
    }
    Particles (const Particles&) = default;
public:
    virtual ~Particles () noexcept = default;
    inline Particles& operator= (const Particles& other)
    {
        std::copy (other.array, other.array + N, array);
        return *this;
    }
    inline Particles& operator= (const T& value)
    {
        std::fill (array, array + N, value);
        return *this;
    }
    inline Particle<T>& operator[] (int index)
    {
        return array[index];
    }
    inline const Particle<T>& operator[] (int index) const
    {
        return array[index];
    }
    Particle<T> *begin()
    {
        return array;
    }
    Particle<T> *end()
    {
        return array + N;
    }
    const Particle<T> *begin() const
    {
        return array;
    }
    const Particle<T> *end() const
    {
        return array + N;
    }
protected:
    Particle<T> *array;
};

template <typename T, int N>
class NewParticles: public Particles<T,N>
{
public:
    NewParticles (): Particles<T,N> (new Particle<T>[N])
    {
        if (config::verbose)
        {
            printf ("NewParticles (Default ctor): Allocated %lu bytes.\n", N*sizeof (T));
        }
    }
    NewParticles (const NewParticles& other): Particles<T,N> (new Particle<T>[N])
    {
        if (config::verbose)
        {
            printf ("NewParticles (Copy ctor): Allocated %lu bytes.\n", N*sizeof (T));
        }
        Particles<T,N>::operator= (other);
    }
    NewParticles& operator= (const NewParticles&) = default;
    NewParticles (NewParticles&& other) noexcept: Particles<T,N> (other.array)
    {
        other.array = nullptr;
    }
    NewParticles& operator= (NewParticles&&) = delete;
    ~NewParticles () noexcept
    {
        if (config::verbose)
        {
            if (this->array != nullptr)
            {
                printf ("NewParticles: Deallocated %lu bytes.\n", N*sizeof (T));
            }
        }
        delete[] this->array;
    }
    using Particles<T,N>::operator=;
};

template <typename T>
using GlobalParticles = Particles<T,vfpic::npar>;

template <typename T>
using LocalParticles = Particles<T,vfpic::mpar>;

template <typename T>
using NewGlobalParticles = NewParticles<T,vfpic::npar>;

template <typename T>
using NewLocalParticles = NewParticles<T,vfpic::mpar>;

template <typename T>
struct LocalParticlesView: public Particles<T,vfpic::mpar>
{
    LocalParticlesView (NewGlobalParticles<T>& array, int ithread):
    Particles<T, vfpic::mpar>(&array[ithread*vfpic::mpar])
    {
    }
};

#endif /* defined(__vf_pic__particle__) */
