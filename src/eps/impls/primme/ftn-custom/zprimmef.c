/*
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2009, Universidad Politecnica de Valencia, Spain

   This file is part of SLEPc.
      
   SLEPc is free software: you can redistribute it and/or modify it under  the
   terms of version 3 of the GNU Lesser General Public License as published by
   the Free Software Foundation.

   SLEPc  is  distributed in the hope that it will be useful, but WITHOUT  ANY 
   WARRANTY;  without even the implied warranty of MERCHANTABILITY or  FITNESS 
   FOR  A  PARTICULAR PURPOSE. See the GNU Lesser General Public  License  for 
   more details.

   You  should have received a copy of the GNU Lesser General  Public  License
   along with SLEPc. If not, see <http://www.gnu.org/licenses/>.
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#include "private/fortranimpl.h"
#include "slepceps.h"

#ifdef PETSC_HAVE_FORTRAN_CAPS
#define epsprimmegetmethod_  EPSPRIMMEGETMETHOD
#define epsprimmegetprecond_ EPSPRIMMEGETPRECOND
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define epsprimmegetmethod_  epsprimmegetmethod
#define epsprimmegetprecond_ epsprimmegetprecond
#endif

EXTERN_C_BEGIN

void PETSC_STDCALL  epsprimmegetmethod_(EPS *eps,EPSPRIMMEMethod *method, PetscErrorCode *ierr ){
  *ierr = EPSPRIMMEGetMethod(*eps,method);
}

void PETSC_STDCALL  epsprimmegetprecond_(EPS *eps,EPSPRIMMEPrecond *precond, PetscErrorCode *ierr ){
  *ierr = EPSPRIMMEGetPrecond(*eps,precond);
}

EXTERN_C_END

