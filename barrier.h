//
//  barrier.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef __vf_pic__barrier__
#define __vf_pic__barrier__

#include <atomic>

class Barrier
{
public:
    Barrier (const int);
    bool wait ();
private:
    const int nthreads;
    std::atomic<int> count;
    std::atomic<bool> generation;
};

#endif /* defined(__vf_pic__barrier__) */
