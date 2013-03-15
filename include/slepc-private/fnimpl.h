/*
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2012, Universitat Politecnica de Valencia, Spain

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

#if !defined(_FNIMPL)
#define _FNIMPL

#include <slepcfn.h>
#include <slepc-private/slepcimpl.h>

typedef struct _FNOps *FNOps;

struct _FNOps {
  PetscErrorCode (*evaluatefunction)(FN,PetscScalar,PetscScalar*);
  PetscErrorCode (*evaluatederivative)(FN,PetscScalar,PetscScalar*);
  PetscErrorCode (*view)(FN,PetscViewer);
};

struct _p_FN {
  PETSCHEADER(struct _FNOps);
  PetscInt    na;
  PetscScalar *alpha;   /* first group of parameters */
  PetscInt    nb;
  PetscScalar *beta;    /* second group of parameters */
};

#endif
