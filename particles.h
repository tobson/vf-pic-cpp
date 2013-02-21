//
//  particle.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/28/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__particle__
#define __vf_pic__particle__

#ifdef DEBUG
#include <cassert>
#endif
#include <iostream>

#include "config.h"
#include "global.h"

/* Declerations */

struct Particle
{
    real x, z;
    real vx, vy, vz;
};

template <int N>
class Particles
{
protected:
    explicit Particles (Particle *ptr): array (ptr)
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
    inline Particles& operator= (const real& value)
    {
        std::fill (array, array + N, value);
        return *this;
    }
    inline Particle& operator[] (int index)
    {
#ifdef DEBUG
        assert (0 <= index < N);
#endif
        return array[index];
    }
    inline const Particle& operator[] (int index) const
    {
#ifdef DEBUG
        assert (0 <= index < N);
#endif
        return array[index];
    }
    Particle *begin()
    {
        return array;
    }
    Particle *end()
    {
        return array + N;
    }
    const Particle *begin() const
    {
        return array;
    }
    const Particle *end() const
    {
        return array + N;
    }
protected:
    Particle *array;
};

template <int N>
class NewParticles: public Particles<N>
{
public:
    NewParticles (): Particles<N> (new Particle[N])
    {
        if (config::verbose)
        {
            printf ("NewParticles (Default ctor): Allocated %lu bytes.\n", N*sizeof (real));
        }
    }
    NewParticles (const NewParticles& other): Particles<N> (new Particle[N])
    {
        if (config::verbose)
        {
            printf ("NewParticles (Copy ctor): Allocated %lu bytes.\n", N*sizeof (real));
        }
        Particles<N>::operator= (other);
    }
    NewParticles& operator= (const NewParticles&) = default;
    NewParticles (NewParticles&& other) noexcept: Particles<N> (other.array)
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
                printf ("NewParticles: Deallocated %lu bytes.\n", N*sizeof (real));
            }
        }
        delete[] this->array;
    }
    using Particles<N>::operator=;
    friend std::ostream& operator<< (std::ostream& os, const NewParticles& particles)
    {
        os.write (reinterpret_cast<char *> (particles.array), N*sizeof (Particle));
        return os;
    }
    friend std::istream& operator>> (std::istream& is, NewParticles& particles)
    {
        is.read (reinterpret_cast<char *> (particles.array), N*sizeof (Particle));
        return is;
    }
};

using GlobalParticles = Particles<vfpic::npar>;

using LocalParticles = Particles<vfpic::mpar>;

using NewGlobalParticles = NewParticles<vfpic::npar>;

using NewLocalParticles = NewParticles<vfpic::mpar>;

struct LocalParticlesView: public Particles<vfpic::mpar>
{
    LocalParticlesView (NewGlobalParticles& array, int ithread):
    Particles<vfpic::mpar>(&array[ithread*vfpic::mpar])
    {
    }
};

#endif /* defined(__vf_pic__particle__) */
