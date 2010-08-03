/*
  SLEPc eigensolver: "jd"

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

#include "private/epsimpl.h"                /*I "slepceps.h" I*/
#include "private/stimpl.h"
#include "../src/eps/impls/davidson/common/davidson.h"
#include "slepcblaslapack.h"

PetscErrorCode EPSSetUp_JD(EPS eps);
PetscErrorCode EPSDestroy_JD(EPS eps);

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "EPSSetFromOptions_JD"
PetscErrorCode EPSSetFromOptions_JD(EPS eps)
{
  PetscErrorCode  ierr;
  PetscTruth      flg,op;
  PetscInt        opi,opi0;
  PetscReal       opf;

  PetscFunctionBegin;
  
  ierr = PetscOptionsBegin(((PetscObject)eps)->comm,((PetscObject)eps)->prefix,"JD Options","EPS");CHKERRQ(ierr);

  ierr = EPSJDGetKrylovStart(eps, &op); CHKERRQ(ierr);
  ierr = PetscOptionsTruth("-eps_jd_krylov_start","Start the searching subspace with a krylov basis","EPSJDSetKrylovStart",op,&op,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetKrylovStart(eps, op); CHKERRQ(ierr); }
 
  ierr = EPSJDGetBlockSize(eps, &opi); CHKERRQ(ierr);
  ierr = PetscOptionsInt("-eps_jd_blocksize","Number vectors add to the searching subspace (if 0, nev employed)","EPSJDSetBlockSize",opi,&opi,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetBlockSize(eps, opi); CHKERRQ(ierr); }

  ierr = EPSJDGetRestart(eps, &opi, &opi0); CHKERRQ(ierr);
  ierr = PetscOptionsInt("-eps_jd_minv","Set the size of the searching subspace after restarting (if 0, eps_jd_bs is employed)","EPSJDSetRestart",opi,&opi,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetRestart(eps, opi, opi0); CHKERRQ(ierr); }

  ierr = PetscOptionsInt("-eps_jd_plusk","Set the number of saved eigenvectors from the previous iteration when restarting","EPSJDSetRestart",opi0,&opi0,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetRestart(eps, opi, opi0); CHKERRQ(ierr); }

  ierr = EPSJDGetInitialSize(eps, &opi); CHKERRQ(ierr);
  ierr = PetscOptionsInt("-eps_jd_initial_size","Set the initial size of the searching subspace","EPSJDSetInitialSize",opi,&opi,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetInitialSize(eps, opi); CHKERRQ(ierr); }

  ierr = EPSJDGetFix(eps, &opf); CHKERRQ(ierr);
  ierr = PetscOptionsReal("-eps_jd_fix","Set the tolerance for changing the target in the correction equation","EPSJDSetFix",opf,&opf,&flg); CHKERRQ(ierr);
  if(flg) { ierr = EPSJDSetFix(eps, opf); CHKERRQ(ierr); }

  ierr = PetscOptionsEnd();CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}  
EXTERN_C_END


#undef __FUNCT__  
#define __FUNCT__ "EPSSetUp_JD"
PetscErrorCode EPSSetUp_JD(EPS eps)
{
  PetscErrorCode  ierr;
  PetscTruth      t;
  KSP             ksp;

  PetscFunctionBegin;

  /* Setup common for all davidson solvers */
  ierr = EPSSetUp_DAVIDSON(eps); CHKERRQ(ierr);

  /* Check some constraints */ 
  ierr = STSetUp(eps->OP); CHKERRQ(ierr);
  ierr = STGetKSP(eps->OP, &ksp); CHKERRQ(ierr);
  ierr = PetscTypeCompare((PetscObject)ksp, KSPPREONLY, &t); CHKERRQ(ierr);
  if (t == PETSC_TRUE) SETERRQ(PETSC_ERR_SUP, "jd does not work with preonly ksp of the spectral transformation");

  PetscFunctionReturn(0);
}


EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "EPSCreate_JD"
PetscErrorCode EPSCreate_JD(EPS eps) {
  PetscErrorCode  ierr;
  KSP             ksp;

  PetscFunctionBegin;

  /* Load the DAVIDSON solver */
  ierr = EPSCreate_DAVIDSON(eps); CHKERRQ(ierr);

  /* Set the default ksp of the st to gmres */
  ierr = STGetKSP(eps->OP, &ksp); CHKERRQ(ierr);
  ierr = KSPSetType(ksp, KSPGMRES); CHKERRQ(ierr);
  ierr = KSPSetTolerances(ksp, 1e-3, 1e-10, PETSC_DEFAULT, 90); CHKERRQ(ierr);

  /* Overload the JD properties */
  eps->ops->setfromoptions       = EPSSetFromOptions_JD;
  eps->ops->setup                = EPSSetUp_JD;
  eps->ops->destroy              = EPSDestroy_JD;

  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetKrylovStart_C","EPSDAVIDSONSetKrylovStart_DAVIDSON",EPSDAVIDSONSetKrylovStart_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetKrylovStart_C","EPSDAVIDSONGetKrylovStart_DAVIDSON",EPSDAVIDSONGetKrylovStart_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetBlockSize_C","EPSDAVIDSONSetBlockSize_DAVIDSON",EPSDAVIDSONSetBlockSize_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetBlockSize_C","EPSDAVIDSONGetBlockSize_DAVIDSON",EPSDAVIDSONGetBlockSize_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetRestart_C","EPSDAVIDSONSetRestart_DAVIDSON",EPSDAVIDSONSetRestart_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetRestart_C","EPSDAVIDSONGetRestart_DAVIDSON",EPSDAVIDSONGetRestart_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetInitialSize_C","EPSDAVIDSONSetInitialSize_DAVIDSON",EPSDAVIDSONSetInitialSize_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetInitialSize_C","EPSDAVIDSONGetInitialSize_DAVIDSON",EPSDAVIDSONGetInitialSize_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetFix_C","EPSDAVIDSONSetFix_DAVIDSON",EPSDAVIDSONSetFix_DAVIDSON);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetFix_C","EPSDAVIDSONGetFix_DAVIDSON",EPSDAVIDSONGetFix_DAVIDSON);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__  
#define __FUNCT__ "EPSDestroy_JD"
PetscErrorCode EPSDestroy_JD(EPS eps)
{
  PetscErrorCode  ierr;

  PetscFunctionBegin;

  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetKrylovStart_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetKrylovStart_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetBlockSize_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetBlockSize_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetRestart_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetRestart_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetInitialSize_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetInitialSize_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDSetFix_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSJDGetFix_C","",PETSC_NULL);CHKERRQ(ierr);

  ierr = EPSDestroy_DAVIDSON(eps);

  PetscFunctionReturn(0);
}


#undef __FUNCT__  
#define __FUNCT__ "EPSJDSetKrylovStart"
/*@
   EPSJDSetKrylovStart - Activates or deactivates starting the searching
   subspace with a Krylov basis. 

   Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
-  krylovstart - boolean flag

   Options Database Key:
.  -eps_jd_krylov_start - Activates starting the searching subspace with a
    Krylov basis
   
   Level: advanced

.seealso: EPSJDGetKrylovStart()
@*/
PetscErrorCode EPSJDSetKrylovStart(EPS eps,PetscTruth krylovstart)
{
  PetscErrorCode ierr, (*f)(EPS,PetscTruth);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDSetKrylovStart_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,krylovstart);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDGetKrylovStart"
/*@
   EPSJDGetKrylovStart - Gets if the searching subspace is started with a
   Krylov basis.

   Collective on EPS

   Input Parameter:
.  eps - the eigenproblem solver context

   Output Parameters:
.  krylovstart - boolean flag indicating if starting the searching subspace
   with a Krylov basis is enabled.

   Level: advanced

.seealso: EPSJDGetKrylovStart()
@*/
PetscErrorCode EPSJDGetKrylovStart(EPS eps,PetscTruth *krylovstart)
{
  PetscErrorCode ierr, (*f)(EPS,PetscTruth*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDGetKrylovStart_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,krylovstart);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDSetBlockSize"
/*@
   EPSJDSetBlockSize - Sets the number of vectors added to the searching space
   every iteration.

   Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
-  blocksize - non-zero positive integer

   Options Database Key:
.  -eps_jd_blocksize - integer indicating the number of vectors added to the
   searching space every iteration. 
   
   Level: advanced

.seealso: EPSJDSetKrylovStart()
@*/
PetscErrorCode EPSJDSetBlockSize(EPS eps,PetscInt blocksize)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDSetBlockSize_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,blocksize);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDGetBlockSize"
/*@
   EPSJDGetBlockSize - Gets the number of vectors added to the searching space
   every iteration.

   Collective on EPS

   Input Parameter:
.  eps - the eigenproblem solver context

   Output Parameter:
.  blocksize - integer indicating the number of vectors added to the searching
   space every iteration.

   Level: advanced

.seealso: EPSJDSetBlockSize()
@*/
PetscErrorCode EPSJDGetBlockSize(EPS eps,PetscInt *blocksize)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDGetBlockSize_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,blocksize);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDGetRestart"
/*@
   EPSJDGetRestart - Gets the number of vectors of the searching space after
   restarting and the number of vectors saved from the previous iteration.

   Collective on EPS

   Input Parameter:
.  eps - the eigenproblem solver context

   Output Parameter:
+  minv - non-zero positive integer indicating the number of vectors of the
   searching subspace after restarting
-  plusk - positive integer indicating the number of vectors saved from the
   previous iteration   

   Level: advanced

.seealso: EPSJDSetRestart()
@*/
PetscErrorCode EPSJDGetRestart(EPS eps,PetscInt *minv,PetscInt *plusk)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt*,PetscInt*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDGetRestart_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,minv,plusk);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDSetRestart"
/*@
   EPSJDSetRestart - Sets the number of vectors of the searching space after
   restarting and the number of vectors saved from the previous iteration.

   Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
.  minv - non-zero positive integer indicating the number of vectors of the
   searching subspace after restarting
-  plusk - positive integer indicating the number of vectors saved from the
   previous iteration   

   Options Database Key:
+  -eps_jd_minv - non-zero positive integer indicating the number of vectors
    of the searching subspace after restarting
-  -eps_jd_plusk - positive integer indicating the number of vectors saved
    from the previous iteration   
   
   Level: advanced

.seealso: EPSJDGetRestart()
@*/
PetscErrorCode EPSJDSetRestart(EPS eps,PetscInt minv,PetscInt plusk)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt,PetscInt);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDSetRestart_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,minv,plusk);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDGetInitialSize"
/*@
   EPSJDGetInitialSize - Gets the initial size of the searching space. In the 
   case of EPSGetKrylovStart is PETSC_FALSE and the user provides vectors by
   EPSSetInitialSpace, up to initialsize vectors will be used; and if the
   provided vectors are not enough, the solver completes the subspace with
   random vectors. In the case of EPSGetKrylovStart is PETSC_TRUE, the solver
   gets the first vector provided by the user or, if not, a random vector,
   and expands the Krylov basis up to initialsize vectors.

   Collective on EPS

   Input Parameter:
.  eps - the eigenproblem solver context

   Output Parameter:
.  initialsize - non-zero positive integer indicating the number of vectors of
   the initial searching subspace

   Level: advanced

.seealso: EPSJDSetInitialSize(), EPSGetKrylovStart()
@*/
PetscErrorCode EPSJDGetInitialSize(EPS eps,PetscInt *initialsize)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDGetInitialSize_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,initialsize);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDSetInitialSize"
/*@
   EPSJDSetInitialSize - Sets the initial size of the searching space. In the 
   case of EPSGetKrylovStart is PETSC_FALSE and the user provides vectors by
   EPSSetInitialSpace, up to initialsize vectors will be used; and if the
   provided vectors are not enough, the solver completes the subspace with
   random vectors. In the case of EPSGetKrylovStart is PETSC_TRUE, the solver
   gets the first vector provided by the user or, if not, a random vector,
   and expands the Krylov basis up to initialsize vectors.

   Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
-  initialsize - non-zero positive integer indicating the number of vectors of
   the initial searching subspace

   Options Database Key:
.  -eps_jd_initial_size - non-zero positive integer indicating the number of
    vectors of the initial searching subspace
   
   Level: advanced

.seealso: EPSJDGetInitialSize(), EPSGetKrylovStart()
@*/
PetscErrorCode EPSJDSetInitialSize(EPS eps,PetscInt initialsize)
{
  PetscErrorCode ierr, (*f)(EPS,PetscInt);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDSetInitialSize_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,initialsize);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDGetFix"
/*@
   EPSJDGetFix - Gets the threshold for changing the target in the correction
   equation. The target in the correction equation is fixed at the first
   iterations. When the norm of the residual vector is lower than this value
   the target is set to the corresponding eigenvalue.

   Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
-  fix - positive float number

   Level: advanced

.seealso: EPSJDSetFix()
@*/
PetscErrorCode EPSJDGetFix(EPS eps,PetscReal *fix)
{
  PetscErrorCode ierr, (*f)(EPS,PetscReal*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDGetFix_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,fix);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSJDSetFix"
/*@
   EPSJDSetFix - Sets the threshold for changing the target in the correction
   equation. The target in the correction equation is fixed at the first
   iterations. When the norm of the residual vector is lower than this value
   the target is set to the corresponding eigenvalue.

   Collective on EPS

   Input Parameter:
.  eps - the eigenproblem solver context

   Output Parameter:
.  fix - positive float number

   Options Database Key:
.  -eps_jd_fix
   
   Level: advanced

.seealso: EPSJDGetFix()
@*/
PetscErrorCode EPSJDSetFix(EPS eps,PetscReal fix)
{
  PetscErrorCode ierr, (*f)(EPS,PetscReal);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)eps,"EPSJDSetFix_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(eps,fix);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}
