/*
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2018, Universitat Politecnica de Valencia, Spain

   This file is part of SLEPc.
   SLEPc is distributed under a 2-clause BSD license (see LICENSE).
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#include <slepc/private/epsimpl.h>  /*I "slepceps.h" I*/

PETSC_EXTERN PetscErrorCode EPSCreate_Power(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_Subspace(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_Arnoldi(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_Lanczos(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_KrylovSchur(EPS);
#if defined(SLEPC_HAVE_ARPACK)
PETSC_EXTERN PetscErrorCode EPSCreate_ARPACK(EPS);
#endif
PETSC_EXTERN PetscErrorCode EPSCreate_LAPACK(EPS);
#if defined(SLEPC_HAVE_BLZPACK)
PETSC_EXTERN PetscErrorCode EPSCreate_BLZPACK(EPS);
#endif
#if defined(SLEPC_HAVE_TRLAN)
PETSC_EXTERN PetscErrorCode EPSCreate_TRLAN(EPS);
#endif
#if defined(SLEPC_HAVE_BLOPEX)
PETSC_EXTERN PetscErrorCode EPSCreate_BLOPEX(EPS);
#endif
#if defined(SLEPC_HAVE_PRIMME)
PETSC_EXTERN PetscErrorCode EPSCreate_PRIMME(EPS);
#endif
#if defined(SLEPC_HAVE_FEAST)
PETSC_EXTERN PetscErrorCode EPSCreate_FEAST(EPS);
#endif
PETSC_EXTERN PetscErrorCode EPSCreate_GD(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_JD(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_RQCG(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_LOBPCG(EPS);
PETSC_EXTERN PetscErrorCode EPSCreate_CISS(EPS);

/*@C
  EPSRegisterAll - Registers all the eigenvalue solvers in the EPS package.

  Not Collective

  Level: advanced

.seealso:  EPSRegister()
@*/
PetscErrorCode EPSRegisterAll(void)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (EPSRegisterAllCalled) PetscFunctionReturn(0);
  EPSRegisterAllCalled = PETSC_TRUE;
  ierr = EPSRegister(EPSKRYLOVSCHUR,EPSCreate_KrylovSchur);CHKERRQ(ierr);
  ierr = EPSRegister(EPSPOWER,EPSCreate_Power);CHKERRQ(ierr);
  ierr = EPSRegister(EPSSUBSPACE,EPSCreate_Subspace);CHKERRQ(ierr);
  ierr = EPSRegister(EPSARNOLDI,EPSCreate_Arnoldi);CHKERRQ(ierr);
  ierr = EPSRegister(EPSLANCZOS,EPSCreate_Lanczos);CHKERRQ(ierr);
  ierr = EPSRegister(EPSGD,EPSCreate_GD);CHKERRQ(ierr);
  ierr = EPSRegister(EPSJD,EPSCreate_JD);CHKERRQ(ierr);
  ierr = EPSRegister(EPSRQCG,EPSCreate_RQCG);CHKERRQ(ierr);
  ierr = EPSRegister(EPSLOBPCG,EPSCreate_LOBPCG);CHKERRQ(ierr);
  ierr = EPSRegister(EPSCISS,EPSCreate_CISS);CHKERRQ(ierr);
  ierr = EPSRegister(EPSLAPACK,EPSCreate_LAPACK);CHKERRQ(ierr);
#if defined(SLEPC_HAVE_ARPACK)
  ierr = EPSRegister(EPSARPACK,EPSCreate_ARPACK);CHKERRQ(ierr);
#endif
#if defined(SLEPC_HAVE_BLZPACK)
  ierr = EPSRegister(EPSBLZPACK,EPSCreate_BLZPACK);CHKERRQ(ierr);
#endif
#if defined(SLEPC_HAVE_TRLAN)
  ierr = EPSRegister(EPSTRLAN,EPSCreate_TRLAN);CHKERRQ(ierr);
#endif
#if defined(SLEPC_HAVE_BLOPEX)
  ierr = EPSRegister(EPSBLOPEX,EPSCreate_BLOPEX);CHKERRQ(ierr);
#endif
#if defined(SLEPC_HAVE_PRIMME)
  ierr = EPSRegister(EPSPRIMME,EPSCreate_PRIMME);CHKERRQ(ierr);
#endif
#if defined(SLEPC_HAVE_FEAST)
  ierr = EPSRegister(EPSFEAST,EPSCreate_FEAST);CHKERRQ(ierr);
#endif
  PetscFunctionReturn(0);
}
