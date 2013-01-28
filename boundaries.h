//
//  boundaries.h
//  vf-pic
//
//  Created by Tobias Heinemann on 1/27/13.
//  Copyright (c) 2013 Tobias Heinemann. All rights reserved.
//

#ifndef vf_pic_boundaries_h
#define vf_pic_boundaries_h

template <typename T, int Nz, int Nx>
boundaryConditionX (ScalarField<T,Nz,Nx>&);
template <typename T, int Nz, int Nx>
boundaryConditionZ (ScalarField<T,Nz,Nx>&);
template <typename T, int Nz, int Nx>
boundaryCondition (ScalarField<T,Nz,Nx>&);

template <typename T, int Nz, int Nx>
boundaryConditionX (VectorField<T,Nz,Nx>&);
template <typename T, int Nz, int Nx>
boundaryConditionZ (VectorField<T,Nz,Nx>&);
template <typename T, int Nz, int Nx>
boundaryCondition (VectorField<T,Nz,Nx>&);

template <typename T, int Nz, int Nx>
boundaryConditionX (ScalarField<T,Nz,Nx>& scalar)
{
  for (int iz = 1; iz <= Nz; ++iz)
  {
    scalar(iz,0   ) = scalar(iz,Nx);
    scalar(iz,Nx+1) = scalar(iz,1 );
  }
}

template <typename T, int Nz, int Nx>
boundaryConditionZ (ScalarField<T,Nz,Nx>& scalar)
{
  for (int ix = 0; ix < Nx + 2; ++ix)
  {
    scalar(0   ,ix) = scalar(Nz,ix);
    scalar(Nz+1,ix) = scalar(1 ,ix);
  }
}

template <typename T, int Nz, int Nx>
boundaryCondition (ScalarField<T,Nz,Nx>& scalar)
{
  boundaryConditionX (scalar);
  boundaryConditionZ (scalar);
}

template <typename T, int Nz, int Nx>
boundaryConditionX (VectorField<T,Nz,Nx>& vector)
{
  boundaryConditionX (vector.x);
  boundaryConditionX (vector.y);
  boundaryConditionX (vector.z);
}

template <typename T, int Nz, int Nx>
boundaryConditionZ (VectorField<T,Nz,Nx>& vector)
{
  boundaryConditionZ (vector.x);
  boundaryConditionZ (vector.y);
  boundaryConditionZ (vector.z);
}

template <typename T, int Nz, int Nx>
boundaryCondition (VectorField<T,Nz,Nx>& vector)
{
  boundaryConditionX (vector);
  boundaryConditionZ (vector);
}

#endif
