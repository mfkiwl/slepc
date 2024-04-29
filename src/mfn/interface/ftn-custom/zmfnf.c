/*
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-, Universitat Politecnica de Valencia, Spain

   This file is part of SLEPc.
   SLEPc is distributed under a 2-clause BSD license (see LICENSE).
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#include <petsc/private/fortranimpl.h>
#include <slepcmfn.h>

#if defined(PETSC_HAVE_FORTRAN_CAPS)
#define mfndestroy_                       MFNDESTROY
#define mfnmonitordefault_                MFNMONITORDEFAULT
#define mfnmonitorset_                    MFNMONITORSET
#define mfngettolerances00_               MFNGETTOLERANCES00
#define mfngettolerances10_               MFNGETTOLERANCES10
#define mfngettolerances01_               MFNGETTOLERANCES01
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define mfndestroy_                       mfndestroy
#define mfnmonitordefault_                mfnmonitordefault
#define mfnmonitorset_                    mfnmonitorset
#define mfngettolerances00_               mfngettolerances00
#define mfngettolerances10_               mfngettolerances10
#define mfngettolerances01_               mfngettolerances01
#endif

/*
   These are not usually called from Fortran but allow Fortran users
   to transparently set these monitors from .F code
*/
SLEPC_EXTERN void mfnmonitordefault_(MFN *mfn,PetscInt *it,PetscReal *errest,PetscViewerAndFormat **ctx,PetscErrorCode *ierr)
{
  *ierr = MFNMonitorDefault(*mfn,*it,*errest,*ctx);
}

static struct {
  PetscFortranCallbackId monitor;
  PetscFortranCallbackId monitordestroy;
} _cb;

/* These are not extern C because they are passed into non-extern C user level functions */
static PetscErrorCode ourmonitor(MFN mfn,PetscInt i,PetscReal d,void* ctx)
{
  PetscObjectUseFortranCallback(mfn,_cb.monitor,(MFN*,PetscInt*,PetscReal*,void*,PetscErrorCode*),(&mfn,&i,&d,_ctx,&ierr));
}

static PetscErrorCode ourdestroy(void** ctx)
{
  MFN mfn = (MFN)*ctx;
  PetscObjectUseFortranCallback(mfn,_cb.monitordestroy,(void*,PetscErrorCode*),(_ctx,&ierr));
}

SLEPC_EXTERN void mfndestroy_(MFN *mfn,PetscErrorCode *ierr)
{
  PETSC_FORTRAN_OBJECT_F_DESTROYED_TO_C_NULL(mfn);
  *ierr = MFNDestroy(mfn); if (*ierr) return;
  PETSC_FORTRAN_OBJECT_C_NULL_TO_F_DESTROYED(mfn);
}

SLEPC_EXTERN void mfnmonitorset_(MFN *mfn,void (*monitor)(MFN*,PetscInt*,PetscReal*,void*,PetscErrorCode*),void *mctx,void (*monitordestroy)(void *,PetscErrorCode*),PetscErrorCode *ierr)
{
  CHKFORTRANNULLOBJECT(mctx);
  CHKFORTRANNULLFUNCTION(monitordestroy);
  if ((PetscVoidFunction)monitor == (PetscVoidFunction)mfnmonitordefault_) {
    *ierr = MFNMonitorSet(*mfn,(PetscErrorCode (*)(MFN,PetscInt,PetscReal,void*))MFNMonitorDefault,*(PetscViewerAndFormat**)mctx,(PetscErrorCode (*)(void**))PetscViewerAndFormatDestroy);
  } else {
    *ierr = PetscObjectSetFortranCallback((PetscObject)*mfn,PETSC_FORTRAN_CALLBACK_CLASS,&_cb.monitor,(PetscVoidFunction)monitor,mctx); if (*ierr) return;
    *ierr = PetscObjectSetFortranCallback((PetscObject)*mfn,PETSC_FORTRAN_CALLBACK_CLASS,&_cb.monitordestroy,(PetscVoidFunction)monitordestroy,mctx); if (*ierr) return;
    *ierr = MFNMonitorSet(*mfn,ourmonitor,*mfn,ourdestroy);
  }
}

SLEPC_EXTERN void mfngettolerances_(MFN *mfn,PetscReal *tol,PetscInt *maxits,PetscErrorCode *ierr)
{
  CHKFORTRANNULLREAL(tol);
  CHKFORTRANNULLINTEGER(maxits);
  *ierr = MFNGetTolerances(*mfn,tol,maxits);
}

SLEPC_EXTERN void mfngettolerances00_(MFN *mfn,PetscReal *tol,PetscInt *maxits,PetscErrorCode *ierr)
{
  mfngettolerances_(mfn,tol,maxits,ierr);
}

SLEPC_EXTERN void mfngettolerances10_(MFN *mfn,PetscReal *tol,PetscInt *maxits,PetscErrorCode *ierr)
{
  mfngettolerances_(mfn,tol,maxits,ierr);
}

SLEPC_EXTERN void mfngettolerances01_(MFN *mfn,PetscReal *tol,PetscInt *maxits,PetscErrorCode *ierr)
{
  mfngettolerances_(mfn,tol,maxits,ierr);
}
