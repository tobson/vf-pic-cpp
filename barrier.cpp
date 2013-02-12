//
//  barrier.cpp
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#include "barrier.h"

#include <thread>

Barrier::Barrier (const int nthreads):
    nthreads (nthreads), count (nthreads), generation (false)
{
}

bool Barrier::wait ()
{
    bool my_generation = generation;
    if (--count)
    {
        while (generation == my_generation) std::this_thread::yield ();
        return false;
    }

    count = nthreads;
    generation =! generation;
    return true;
}