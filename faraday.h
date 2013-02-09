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

template<typename T, int Nz, int Nx>
void faraday (const ScalarBase<T,Nz,Nx>& Aold, const ScalarBase<T,Nz,Nx>& E, const real dt, ScalarBase<T,Nz,Nx>* pAnew)
{
    ScalarBase<T,Nz,Nx>& Anew = *pAnew;
    
    for (int k = 1; k <= Nz; ++k)
    for (int i = 1; i <= Nx; ++i)
    {
        Anew (k,i) = Aold (k,i) - dt*E (k,i);
    }
}

template<typename T, int Nz, int Nx>
void faraday (const VectorBase<T,Nz,Nx>& Aold,
              const VectorBase<T,Nz,Nx>& E, const real dt, VectorBase<T,Nz,Nx>* pAnew)
{
    VectorBase<T,Nz,Nx>& Anew = *pAnew;
    
    for (int j = 0; j < 3; ++j) faraday (Aold[j], E[j], dt, &Anew[j]);
}


#endif
