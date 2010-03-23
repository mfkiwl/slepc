/*                       

   Straightforward linearization for quadratic eigenproblems.

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

#include "private/qepimpl.h"         /*I "slepcqep.h" I*/
#include "slepceps.h"
#include "linearp.h"

#undef __FUNCT__  
#define __FUNCT__ "QEPSetUp_LINEAR"
PetscErrorCode QEPSetUp_LINEAR(QEP qep)
{
  PetscErrorCode    ierr;
  QEP_LINEAR        *ctx = (QEP_LINEAR *)qep->data;
  PetscInt          i,M,N,m,n;
  EPSWhich          which;
  /* function tables */
  PetscErrorCode (*fcreate[][2])(MPI_Comm,QEP_LINEAR*,Mat*) = {
    { MatCreateExplicit_QEPLINEAR_N1A, MatCreateExplicit_QEPLINEAR_N1B }    /* N1 */
  };
  PetscErrorCode (*fmult[][2])(Mat,Vec,Vec) = {
    { MatMult_QEPLINEAR_N1A, MatMult_QEPLINEAR_N1B }
  };
  PetscErrorCode (*fgetdiagonal[][2])(Mat,Vec) = {
    { MatGetDiagonal_QEPLINEAR_N1A, MatGetDiagonal_QEPLINEAR_N1B }
  };

  PetscFunctionBegin;
  
  if (ctx->A) { 
    ierr = MatDestroy(ctx->A);CHKERRQ(ierr);
    ierr = MatDestroy(ctx->B);CHKERRQ(ierr);
  }
  if (ctx->x1) { 
    ierr = VecDestroy(ctx->x1);CHKERRQ(ierr); 
    ierr = VecDestroy(ctx->x2);CHKERRQ(ierr); 
    ierr = VecDestroy(ctx->y1);CHKERRQ(ierr); 
    ierr = VecDestroy(ctx->y2);CHKERRQ(ierr); 
  }

  i = 0;

  ierr = MatGetSize(ctx->M,&M,&N);CHKERRQ(ierr);
  ierr = MatGetLocalSize(ctx->M,&m,&n);CHKERRQ(ierr);
  if (ctx->explicitmatrix) {
    ctx->x1 = ctx->x2 = ctx->y1 = ctx->y2 = PETSC_NULL;
    ierr = (*fcreate[0][0])(((PetscObject)qep)->comm,ctx,&ctx->A);CHKERRQ(ierr);
    ierr = (*fcreate[0][1])(((PetscObject)qep)->comm,ctx,&ctx->B);CHKERRQ(ierr);
  } else {
    ierr = VecCreateMPIWithArray(((PetscObject)qep)->comm,m,M,PETSC_NULL,&ctx->x1);CHKERRQ(ierr);
    ierr = VecCreateMPIWithArray(((PetscObject)qep)->comm,n,N,PETSC_NULL,&ctx->x2);CHKERRQ(ierr);
    ierr = VecCreateMPIWithArray(((PetscObject)qep)->comm,m,M,PETSC_NULL,&ctx->y1);CHKERRQ(ierr);
    ierr = VecCreateMPIWithArray(((PetscObject)qep)->comm,n,N,PETSC_NULL,&ctx->y2);CHKERRQ(ierr);
    ierr = MatCreateShell(((PetscObject)qep)->comm,m+n,m+n,M+N,M+N,ctx,&ctx->A);CHKERRQ(ierr);
    ierr = MatShellSetOperation(ctx->A,MATOP_MULT,(void(*)(void))fmult[0][0]);CHKERRQ(ierr);
    ierr = MatShellSetOperation(ctx->A,MATOP_GET_DIAGONAL,(void(*)(void))fgetdiagonal[0][0]);CHKERRQ(ierr);
    ierr = MatCreateShell(((PetscObject)qep)->comm,m+n,m+n,M+N,M+N,ctx,&ctx->B);CHKERRQ(ierr);
    ierr = MatShellSetOperation(ctx->B,MATOP_MULT,(void(*)(void))fmult[0][1]);CHKERRQ(ierr);
    ierr = MatShellSetOperation(ctx->B,MATOP_GET_DIAGONAL,(void(*)(void))fgetdiagonal[0][1]);CHKERRQ(ierr);
  }

  ierr = EPSSetOperators(ctx->eps,ctx->A,ctx->B);CHKERRQ(ierr);
  ierr = EPSSetProblemType(ctx->eps,EPS_GNHEP);CHKERRQ(ierr);
  switch (qep->which) {
      case QEP_LARGEST_MAGNITUDE:  which = EPS_LARGEST_MAGNITUDE; break;
      case QEP_SMALLEST_MAGNITUDE: which = EPS_SMALLEST_MAGNITUDE; break;
      case QEP_LARGEST_REAL:       which = EPS_LARGEST_REAL; break;
      case QEP_SMALLEST_REAL:      which = EPS_SMALLEST_REAL; break;
      case QEP_LARGEST_IMAGINARY:  which = EPS_LARGEST_IMAGINARY; break;
      case QEP_SMALLEST_IMAGINARY: which = EPS_SMALLEST_IMAGINARY; break;
  }
  ierr = EPSSetWhichEigenpairs(ctx->eps,which);CHKERRQ(ierr);
  ierr = EPSSetDimensions(ctx->eps,qep->nev,qep->ncv,qep->mpd);CHKERRQ(ierr);
  ierr = EPSSetTolerances(ctx->eps,qep->tol,qep->max_it);CHKERRQ(ierr);
  ierr = EPSSetUp(ctx->eps);CHKERRQ(ierr);
  ierr = EPSGetDimensions(ctx->eps,PETSC_NULL,&qep->ncv,&qep->mpd);CHKERRQ(ierr);
  ierr = EPSGetTolerances(ctx->eps,&qep->tol,&qep->max_it);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "QEPLoadEigenpairsFromEPS"
/*
   QEPLoadEigenpairsFromEPS - Auxiliary routine that copies the solution of the
   linear eigenproblem to the QEP object. The eigenvector of the generalized 
   problem is supposed to be
                               z = [  x  ]
                                   [ l*x ]
   If |l|<1.0, the eigenvector is taken from z(1:n), otherwise from z(n+1:2*n).
   Finally, x is normalized so that ||x||_2 = 1.

   If explicitmatrix==PETSC_TRUE then z is partitioned across processors, otherwise x is.
*/
PetscErrorCode QEPLoadEigenpairsFromEPS(QEP qep,EPS eps,PetscTruth explicitmatrix)
{
  PetscErrorCode ierr;
  PetscInt       i,N,n,start,end,offset,idx;
  PetscScalar    *px;
#if !defined(PETSC_USE_COMPLEX)
  PetscScalar    tmp;
  PetscReal      norm,normi;
#endif
  Vec            v0,xr,xi,w;
  IS             isV1,isV2;
  VecScatter     vsV,vsV1,vsV2;
  
  PetscFunctionBegin;
  ierr = EPSGetInitialVector(eps,&v0);CHKERRQ(ierr);
  ierr = VecDuplicate(v0,&xr);CHKERRQ(ierr);
  ierr = VecDuplicate(v0,&xi);CHKERRQ(ierr);
  ierr = MatGetLocalSize(qep->M,&n,PETSC_NULL);CHKERRQ(ierr);
  ierr = MatGetSize(qep->M,&N,PETSC_NULL);CHKERRQ(ierr);

  if (explicitmatrix) {  /* case 1: x needs to be scattered from the owning processes to the rest */
    ierr = VecGetOwnershipRange(qep->V[0],&start,&end);CHKERRQ(ierr);
    idx = start;
    ierr = ISCreateBlock(((PetscObject)qep)->comm,end-start,1,&idx,&isV1);CHKERRQ(ierr);      
    ierr = VecScatterCreate(xr,isV1,qep->V[0],PETSC_NULL,&vsV1);CHKERRQ(ierr);
    idx = start+N;
    ierr = ISCreateBlock(((PetscObject)qep)->comm,end-start,1,&idx,&isV2);CHKERRQ(ierr);      
    ierr = VecScatterCreate(xr,isV2,qep->V[0],PETSC_NULL,&vsV2);CHKERRQ(ierr);
    for (i=0;i<qep->nconv;i++) {
      ierr = EPSGetEigenpair(eps,i,&qep->eigr[i],&qep->eigi[i],xr,xi);CHKERRQ(ierr);
      if (SlepcAbsEigenvalue(qep->eigr[i],qep->eigi[i])>1.0) vsV = vsV2;
      else vsV = vsV1;
#if !defined(PETSC_USE_COMPLEX)
      if (qep->eigi[i]>0.0) {   /* first eigenvalue of a complex conjugate pair */
        ierr = VecScatterBegin(vsV,xr,qep->V[i],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
        ierr = VecScatterEnd(vsV,xr,qep->V[i],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
        ierr = VecScatterBegin(vsV,xi,qep->V[i+1],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
        ierr = VecScatterEnd(vsV,xi,qep->V[i+1],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
      }
      else if (qep->eigi[i]==0.0)   /* real eigenvalue */
#endif
      {
        ierr = VecScatterBegin(vsV,xr,qep->V[i],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
        ierr = VecScatterEnd(vsV,xr,qep->V[i],INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
      }
    }
    ierr = ISDestroy(isV1);CHKERRQ(ierr);
    ierr = ISDestroy(isV2);CHKERRQ(ierr);
    ierr = VecScatterDestroy(vsV1);CHKERRQ(ierr);
    ierr = VecScatterDestroy(vsV2);CHKERRQ(ierr);
  }
  else {           /* case 2: elements of x are already in the right process */
    ierr = VecCreateMPIWithArray(((PetscObject)qep)->comm,n,N,PETSC_NULL,&w);CHKERRQ(ierr);
    for (i=0;i<qep->nconv;i++) {
      ierr = EPSGetEigenpair(eps,i,&qep->eigr[i],&qep->eigi[i],xr,xi);CHKERRQ(ierr);
      if (SlepcAbsEigenvalue(qep->eigr[i],qep->eigi[i])>1.0) offset = n;
      else offset = 0;
#if !defined(PETSC_USE_COMPLEX)
      if (qep->eigi[i]>0.0) {   /* first eigenvalue of a complex conjugate pair */
        ierr = VecGetArray(xr,&px);CHKERRQ(ierr);
        ierr = VecPlaceArray(w,px+offset);CHKERRQ(ierr);
        ierr = VecCopy(w,qep->V[i]);CHKERRQ(ierr);
        ierr = VecResetArray(w);CHKERRQ(ierr);
        ierr = VecRestoreArray(xr,&px);CHKERRQ(ierr);
        ierr = VecGetArray(xi,&px);CHKERRQ(ierr);
        ierr = VecPlaceArray(w,px+offset);CHKERRQ(ierr);
        ierr = VecCopy(w,qep->V[i+1]);CHKERRQ(ierr);
        ierr = VecResetArray(w);CHKERRQ(ierr);
        ierr = VecRestoreArray(xi,&px);CHKERRQ(ierr);
      }
      else if (qep->eigi[i]==0.0)   /* real eigenvalue */
#endif
      {
        ierr = VecGetArray(xr,&px);CHKERRQ(ierr);
        ierr = VecPlaceArray(w,px+offset);CHKERRQ(ierr);
        ierr = VecCopy(w,qep->V[i]);CHKERRQ(ierr);
        ierr = VecResetArray(w);CHKERRQ(ierr);
        ierr = VecRestoreArray(xr,&px);CHKERRQ(ierr);
      }
    }
    ierr = VecDestroy(w);CHKERRQ(ierr);
  }
  ierr = VecDestroy(xr);CHKERRQ(ierr);
  ierr = VecDestroy(xi);CHKERRQ(ierr);

  /* Normalize eigenvector */
  for (i=0;i<qep->nconv;i++) {
#if !defined(PETSC_USE_COMPLEX)
    if (qep->eigi[i] != 0.0) {
      ierr = VecNorm(qep->V[i],NORM_2,&norm);CHKERRQ(ierr);
      ierr = VecNorm(qep->V[i+1],NORM_2,&normi);CHKERRQ(ierr);
      tmp = 1.0 / SlepcAbsEigenvalue(norm,normi);
      ierr = VecScale(qep->V[i],tmp);CHKERRQ(ierr);
      ierr = VecScale(qep->V[i+1],tmp);CHKERRQ(ierr);
      i++;     
    } else
#endif
    {
      ierr = VecNormalize(qep->V[i],PETSC_NULL);CHKERRQ(ierr);
    }
  }

  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "QEPSolve_LINEAR"
PetscErrorCode QEPSolve_LINEAR(QEP qep)
{
  PetscErrorCode ierr;
  QEP_LINEAR     *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  ierr = EPSSolve(ctx->eps);CHKERRQ(ierr);
  ierr = EPSGetConverged(ctx->eps,&qep->nconv);CHKERRQ(ierr);
  ierr = EPSGetIterationNumber(ctx->eps,&qep->its);CHKERRQ(ierr);
  ierr = EPSGetConvergedReason(ctx->eps,(EPSConvergedReason*)&qep->reason);CHKERRQ(ierr);
  ierr = EPSGetOperationCounters(ctx->eps,&qep->matvecs,PETSC_NULL,&qep->linits);CHKERRQ(ierr);
  qep->matvecs *= 2;  /* convention: count one matvec for each non-trivial block in A */
  ierr = QEPLoadEigenpairsFromEPS(qep,ctx->eps,ctx->explicitmatrix);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSMonitor_QEP_LINEAR"
PetscErrorCode EPSMonitor_QEP_LINEAR(EPS eps,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,void *ctx)
{
  PetscInt   i;
  QEP        qep = (QEP)ctx;

  PetscFunctionBegin;
  nconv = 0;
  for (i=0;i<nest;i++) {
    qep->eigr[i] = eigr[i];
    qep->eigi[i] = eigi[i];
    qep->errest[i] = errest[i];
    if (errest[i] < qep->tol) nconv++;
  }
  QEPMonitor(qep,its,nconv,qep->eigr,qep->eigi,qep->errest,nest);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "QEPSetFromOptions_LINEAR"
PetscErrorCode QEPSetFromOptions_LINEAR(QEP qep)
{
  PetscErrorCode ierr;
  PetscTruth     set;
  PetscInt       i;
  QEP_LINEAR     *ctx = (QEP_LINEAR *)qep->data;
  ST             st;

  PetscFunctionBegin;
  ierr = PetscOptionsBegin(((PetscObject)qep)->comm,((PetscObject)qep)->prefix,"LINEAR Quadratic Eigenvalue Problem solver Options","QEP");CHKERRQ(ierr);

  ierr = PetscOptionsInt("-qep_linear_cform","Number of the companion form","QEPLinearSetCompanionForm",ctx->cform,&i,&set);CHKERRQ(ierr);
  if (set) {
    ierr = QEPLinearSetCompanionForm(qep,i);CHKERRQ(ierr);
  }

  ierr = PetscOptionsTruth("-qep_linear_explicitmatrix","Use explicit matrix in linearization","QEPLinearSetExplicitMatrix",PETSC_FALSE,&ctx->explicitmatrix,PETSC_NULL);CHKERRQ(ierr);
  if (!ctx->explicitmatrix) {
    /* use as default an ST with shell matrix and Jacobi */ 
    ierr = EPSGetST(ctx->eps,&st);CHKERRQ(ierr);
    ierr = STSetMatMode(st,STMATMODE_SHELL);CHKERRQ(ierr);
  }

  ierr = PetscOptionsEnd();CHKERRQ(ierr);
  ierr = EPSSetFromOptions(ctx->eps);CHKERRQ(ierr);
  ierr = PetscOptionsTail();CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearSetCompanionForm_LINEAR"
PetscErrorCode QEPLinearSetCompanionForm_LINEAR(QEP qep,PetscInt cform)
{
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  if (cform==PETSC_IGNORE) PetscFunctionReturn(0);
  if (cform==PETSC_DECIDE || cform==PETSC_DEFAULT) ctx->cform = 1;
  else {
    if (cform!=1 && cform!=2) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,"Invalid value of argument 'cform'");
    ctx->cform = cform;
  }
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__
#define __FUNCT__ "QEPLinearSetCompanionForm"
/*@
   QEPLinearSetCompanionForm - Choose between the two companion forms available
   for the linearization of the quadratic problem.

   Collective on QEP

   Input Parameters:
+  qep   - quadratic eigenvalue solver
-  cform - 1 or 2 (first or second companion form)

   Options Database Key:
.  -qep_linear_cform <int> - Choose the companion form

   Level: advanced

.seealso: QEPLinearGetCompanionForm()
@*/
PetscErrorCode QEPLinearSetCompanionForm(QEP qep,PetscInt cform)
{
  PetscErrorCode ierr, (*f)(QEP,PetscTruth);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearSetCompanionForm_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,cform);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearGetCompanionForm_LINEAR"
PetscErrorCode QEPLinearGetCompanionForm_LINEAR(QEP qep,PetscInt *cform)
{
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  PetscValidPointer(cform,2);
  *cform = ctx->cform;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__
#define __FUNCT__ "QEPLinearGetCompanionForm"
/*@C
   QEPLinearGetCompanionForm - Returns the number of the companion form that
   will be used for the linearization of the quadratic problem.

   Not collective

   Input Parameter:
.  qep  - quadratic eigenvalue solver

   Output Parameter:
.  cform - the companion form number (1 or 2)

   Level: advanced

.seealso: QEPLinearSetCompanionForm()
@*/
PetscErrorCode QEPLinearGetCompanionForm(QEP qep,PetscInt *cform)
{
  PetscErrorCode ierr, (*f)(QEP,PetscInt*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearGetCompanionForm_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,cform);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearSetExplicitMatrix_LINEAR"
PetscErrorCode QEPLinearSetExplicitMatrix_LINEAR(QEP qep,PetscTruth explicitmatrix)
{
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  ctx->explicitmatrix = explicitmatrix;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__
#define __FUNCT__ "QEPLinearSetExplicitMatrix"
/*@
   QEPLinearSetExplicitMatrix - Indicate if the matrices A and B for the linearization
   of the quadratic problem must be built explicitly.

   Collective on QEP

   Input Parameters:
+  qep      - quadratic eigenvalue solver
-  explicit - boolean flag indicating if the matrices are built explicitly

   Options Database Key:
.  -qep_linear_explicitmatrix <boolean> - Indicates the boolean flag

   Level: advanced

.seealso: QEPLinearGetExplicitMatrix()
@*/
PetscErrorCode QEPLinearSetExplicitMatrix(QEP qep,PetscTruth explicitmatrix)
{
  PetscErrorCode ierr, (*f)(QEP,PetscTruth);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearSetExplicitMatrix_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,explicitmatrix);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearGetExplicitMatrix_LINEAR"
PetscErrorCode QEPLinearGetExplicitMatrix_LINEAR(QEP qep,PetscTruth *explicitmatrix)
{
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  PetscValidPointer(explicitmatrix,2);
  *explicitmatrix = ctx->explicitmatrix;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__
#define __FUNCT__ "QEPLinearGetExplicitMatrix"
/*@C
   QEPLinearGetExplicitMatrix - Returns the flag indicating if the matrices A and B
   for the linearization of the quadratic problem are built explicitly.

   Not collective

   Input Parameter:
.  qep  - quadratic eigenvalue solver

   Output Parameter:
.  explicitmatrix - the mode flag

   Level: advanced

.seealso: QEPLinearSetExplicitMatrix()
@*/
PetscErrorCode QEPLinearGetExplicitMatrix(QEP qep,PetscTruth *explicitmatrix)
{
  PetscErrorCode ierr, (*f)(QEP,PetscTruth*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearGetExplicitMatrix_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,explicitmatrix);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearSetEPS_LINEAR"
PetscErrorCode QEPLinearSetEPS_LINEAR(QEP qep,EPS eps)
{
  PetscErrorCode  ierr;
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_COOKIE,2);
  PetscCheckSameComm(qep,1,eps,2);
  ierr = PetscObjectReference((PetscObject)eps);CHKERRQ(ierr);
  ierr = EPSDestroy(ctx->eps);CHKERRQ(ierr);  
  ctx->eps = eps;
  qep->setupcalled = 0;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__  
#define __FUNCT__ "QEPLinearSetEPS"
/*@
   QEPLinearSetEPS - Associate an eigensolver object (EPS) to the
   quadratic eigenvalue solver. 

   Collective on QEP

   Input Parameters:
+  qep - quadratic eigenvalue solver
-  eps - the eigensolver object

   Level: advanced

.seealso: QEPLinearGetEPS()
@*/
PetscErrorCode QEPLinearSetEPS(QEP qep,EPS eps)
{
  PetscErrorCode ierr, (*f)(QEP,EPS);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearSetEPS_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,eps);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPLinearGetEPS_LINEAR"
PetscErrorCode QEPLinearGetEPS_LINEAR(QEP qep,EPS *eps)
{
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  PetscValidPointer(eps,2);
  *eps = ctx->eps;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__  
#define __FUNCT__ "QEPLinearGetEPS"
/*@C
   QEPLinearGetEPS - Retrieve the eigensolver object (EPS) associated
   to the quadratic eigenvalue solver.

   Not Collective

   Input Parameter:
.  qep - quadratic eigenvalue solver

   Output Parameter:
.  eps - the eigensolver object

   Level: advanced

.seealso: QEPLinearSetEPS()
@*/
PetscErrorCode QEPLinearGetEPS(QEP qep,EPS *eps)
{
  PetscErrorCode ierr, (*f)(QEP,EPS*);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(qep,QEP_COOKIE,1);
  ierr = PetscObjectQueryFunction((PetscObject)qep,"QEPLinearGetEPS_C",(void (**)())&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(qep,eps);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "QEPView_LINEAR"
PetscErrorCode QEPView_LINEAR(QEP qep,PetscViewer viewer)
{
  PetscErrorCode  ierr;
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  if (ctx->explicitmatrix) {
    ierr = PetscViewerASCIIPrintf(viewer,"linearized matrices: explicit\n");CHKERRQ(ierr);
  } else {
    ierr = PetscViewerASCIIPrintf(viewer,"linearized matrices: implicit\n");CHKERRQ(ierr);
  }
  ierr = PetscViewerASCIIPrintf(viewer,"companion form: %d\n",ctx->cform);CHKERRQ(ierr);
  ierr = EPSView(ctx->eps,viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "QEPDestroy_LINEAR"
PetscErrorCode QEPDestroy_LINEAR(QEP qep)
{
  PetscErrorCode  ierr;
  QEP_LINEAR *ctx = (QEP_LINEAR *)qep->data;

  PetscFunctionBegin;
  ierr = EPSDestroy(ctx->eps);CHKERRQ(ierr);
  if (ctx->A) {
    ierr = MatDestroy(ctx->A);CHKERRQ(ierr);
    ierr = MatDestroy(ctx->B);CHKERRQ(ierr);
  }
  if (ctx->x1) { 
    ierr = VecDestroy(ctx->x1);CHKERRQ(ierr);
    ierr = VecDestroy(ctx->x2);CHKERRQ(ierr);
    ierr = VecDestroy(ctx->y1);CHKERRQ(ierr);
    ierr = VecDestroy(ctx->y2);CHKERRQ(ierr);
  }
  ierr = QEPDestroy_Default(qep);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "QEPCreate_LINEAR"
PetscErrorCode QEPCreate_LINEAR(QEP qep)
{
  PetscErrorCode ierr;
  QEP_LINEAR     *ctx;

  PetscFunctionBegin;
  ierr = PetscNew(QEP_LINEAR,&ctx);CHKERRQ(ierr);
  PetscLogObjectMemory(qep,sizeof(QEP_LINEAR));
  qep->data                      = (void *)ctx;
  qep->ops->solve                = QEPSolve_LINEAR;
  qep->ops->setup                = QEPSetUp_LINEAR;
  qep->ops->setfromoptions       = QEPSetFromOptions_LINEAR;
  qep->ops->destroy              = QEPDestroy_LINEAR;
  qep->ops->view                 = QEPView_LINEAR;
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearSetCompanionForm_C","QEPLinearSetCompanionForm_LINEAR",QEPLinearSetCompanionForm_LINEAR);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearGetCompanionForm_C","QEPLinearGetCompanionForm_LINEAR",QEPLinearGetCompanionForm_LINEAR);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearSetEPS_C","QEPLinearSetEPS_LINEAR",QEPLinearSetEPS_LINEAR);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearGetEPS_C","QEPLinearGetEPS_LINEAR",QEPLinearGetEPS_LINEAR);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearSetExplicitMatrix_C","QEPLinearSetExplicitMatrix_LINEAR",QEPLinearSetExplicitMatrix_LINEAR);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)qep,"QEPLinearGetExplicitMatrix_C","QEPLinearGetExplicitMatrix_LINEAR",QEPLinearGetExplicitMatrix_LINEAR);CHKERRQ(ierr);

  ierr = EPSCreate(((PetscObject)qep)->comm,&ctx->eps);CHKERRQ(ierr);
  ierr = EPSSetOptionsPrefix(ctx->eps,((PetscObject)qep)->prefix);CHKERRQ(ierr);
  ierr = EPSAppendOptionsPrefix(ctx->eps,"qep_");CHKERRQ(ierr);
  ierr = PetscObjectIncrementTabLevel((PetscObject)ctx->eps,(PetscObject)qep,1);CHKERRQ(ierr);  
  PetscLogObjectParent(qep,ctx->eps);
  ierr = EPSSetIP(ctx->eps,qep->ip);CHKERRQ(ierr);
  ierr = EPSMonitorSet(ctx->eps,EPSMonitor_QEP_LINEAR,qep,PETSC_NULL);CHKERRQ(ierr);
  ctx->M = qep->M;
  ctx->C = qep->C;
  ctx->K = qep->K;
  ctx->explicitmatrix = PETSC_FALSE;
  ctx->cform = 1;
  ctx->A = PETSC_NULL;
  ctx->B = PETSC_NULL;
  ctx->x1 = PETSC_NULL;
  ctx->x2 = PETSC_NULL;
  ctx->y1 = PETSC_NULL;
  ctx->y2 = PETSC_NULL;
  PetscFunctionReturn(0);
}
EXTERN_C_END

