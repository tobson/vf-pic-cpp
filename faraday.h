//
//  faraday.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/30/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_faraday_h
#define vf_pic_faraday_h

#include "vector-field.h"

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void faraday (const S1<T,Nz,Nx>& Aold, const S2<T,Nz,Nx>& E, const real dt, S3<T,Nz,Nx>* pAnew)
{
    S3<T,Nz,Nx>& Anew = *pAnew;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        Anew (k,i) = Aold (k,i) - dt*E (k,i);
    }
}

template<
template<typename, int, int> class S1,
template<typename, int, int> class S2,
template<typename, int, int> class S3, typename T, int Nz, int Nx>
void faraday (const ThreeVector<S1,T,Nz,Nx>& Aold,
              const ThreeVector<S2,T,Nz,Nx>& E, const real dt, ThreeVector<S3,T,Nz,Nx>* pAnew)
{
    ThreeVector<S3,T,Nz,Nx>& Anew = *pAnew;
    
    for (int j = 0; j < 3; ++j) faraday (Aold[j], E[j], dt, &Anew[j]);
}


#endif
