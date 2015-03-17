/*

   SLEPc quadratic eigensolver: "qarnoldi"

   Method: Q-Arnoldi

   Algorithm:

       Quadratic Arnoldi with Krylov-Schur type restart.

   References:

       [1] K. Meerbergen, "The Quadratic Arnoldi method for the solution
           of the quadratic eigenvalue problem", SIAM J. Matrix Anal.
           Appl. 30(4):1462-1482, 2008.

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2014, Universitat Politecnica de Valencia, Spain

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

#include <slepc-private/pepimpl.h>    /*I "slepcpep.h" I*/
#include <petscblaslapack.h>

typedef struct {
  PetscReal keep;         /* restart parameter */
  PetscBool lock;         /* locking/non-locking variant */
} PEP_QARNOLDI;

#undef __FUNCT__
#define __FUNCT__ "PEPSetUp_QArnoldi"
PetscErrorCode PEPSetUp_QArnoldi(PEP pep)
{
  PetscErrorCode ierr;
  PEP_QARNOLDI   *ctx = (PEP_QARNOLDI*)pep->data;
  PetscBool      sinv,flg;

  PetscFunctionBegin;
  ierr = PEPSetDimensions_Default(pep,pep->nev,&pep->ncv,&pep->mpd);CHKERRQ(ierr);
  if (!ctx->lock && pep->mpd<pep->ncv) SETERRQ(PetscObjectComm((PetscObject)pep),PETSC_ERR_SUP,"Should not use mpd parameter in non-locking variant");
  if (!pep->max_it) pep->max_it = PetscMax(100,2*pep->n/pep->ncv);
  if (!pep->which) {
    ierr = PetscObjectTypeCompare((PetscObject)pep->st,STSINVERT,&sinv);CHKERRQ(ierr);
    if (sinv) pep->which = PEP_TARGET_MAGNITUDE;
    else pep->which = PEP_LARGEST_MAGNITUDE;
  }

  if (pep->nmat!=3) SETERRQ(PetscObjectComm((PetscObject)pep),PETSC_ERR_SUP,"Solver only available for quadratic problems");
  if (pep->basis!=PEP_BASIS_MONOMIAL) SETERRQ(PetscObjectComm((PetscObject)pep),PETSC_ERR_SUP,"Solver not implemented for non-monomial bases");
  ierr = STGetTransform(pep->st,&flg);CHKERRQ(ierr);
  if (!flg) SETERRQ(PetscObjectComm((PetscObject)pep),PETSC_ERR_SUP,"Solver requires the ST transformation flag set, see STSetTransform()");

  if (!ctx->keep) ctx->keep = 0.5;

  ierr = PEPAllocateSolution(pep,0);CHKERRQ(ierr);
  ierr = PEPSetWorkVecs(pep,4);CHKERRQ(ierr);

  ierr = DSSetType(pep->ds,DSNHEP);CHKERRQ(ierr);
  ierr = DSSetExtraRow(pep->ds,PETSC_TRUE);CHKERRQ(ierr);
  ierr = DSAllocate(pep->ds,pep->ncv+1);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiCGS"
/*
  Compute a step of Classical Gram-Schmidt orthogonalization
*/
static PetscErrorCode PEPQArnoldiCGS(PEP pep,PetscScalar *H,PetscBLASInt ldh,PetscScalar *h,PetscBLASInt j,BV V,Vec t,Vec v,Vec w,PetscReal *onorm,PetscReal *norm,PetscScalar *work)
{
  PetscErrorCode ierr;
  PetscBLASInt   ione = 1,j_1 = j+1;
  PetscReal      x,y;
  PetscScalar    dot,one = 1.0,zero = 0.0;

  PetscFunctionBegin;
  /* compute norm of v and w */
  if (onorm) {
    ierr = VecNorm(v,NORM_2,&x);CHKERRQ(ierr);
    ierr = VecNorm(w,NORM_2,&y);CHKERRQ(ierr);
    *onorm = PetscSqrtReal(x*x+y*y);
  }

  /* orthogonalize: compute h */
  ierr = BVDotVec(V,v,h);CHKERRQ(ierr);
  ierr = BVDotVec(V,w,work);CHKERRQ(ierr);
  if (j>0)
    PetscStackCallBLAS("BLASgemv",BLASgemv_("C",&j_1,&j,&one,H,&ldh,work,&ione,&one,h,&ione));
  ierr = VecDot(w,t,&dot);CHKERRQ(ierr);
  h[j] += dot;

  /* orthogonalize: update v and w */
  ierr = BVMultVec(V,-1.0,1.0,v,h);CHKERRQ(ierr);
  if (j>0) {
    PetscStackCallBLAS("BLASgemv",BLASgemv_("N",&j_1,&j,&one,H,&ldh,h,&ione,&zero,work,&ione));
    ierr = BVMultVec(V,-1.0,1.0,w,work);CHKERRQ(ierr);
  }
  ierr = VecAXPY(w,-h[j],t);CHKERRQ(ierr);

  /* compute norm of v and w */
  if (norm) {
    ierr = VecNorm(v,NORM_2,&x);CHKERRQ(ierr);
    ierr = VecNorm(w,NORM_2,&y);CHKERRQ(ierr);
    *norm = PetscSqrtReal(x*x+y*y);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldi"
/*
  Compute a run of Q-Arnoldi iterations
*/
static PetscErrorCode PEPQArnoldi(PEP pep,PetscScalar *H,PetscInt ldh,PetscInt k,PetscInt *M,Vec v,Vec w,PetscReal *beta,PetscBool *breakdown,PetscScalar *work)
{
  PetscErrorCode     ierr;
  PetscInt           i,j,l,m = *M;
  Vec                t = pep->work[2],u = pep->work[3];
  BVOrthogRefineType refinement;
  PetscReal          norm,onorm,eta;
  PetscScalar        *c = work + m;

  PetscFunctionBegin;
  ierr = BVGetOrthogonalization(pep->V,NULL,&refinement,&eta);CHKERRQ(ierr);
  ierr = BVInsertVec(pep->V,k,v);CHKERRQ(ierr);
  for (j=k;j<m;j++) {
    /* apply operator */
    ierr = VecCopy(w,t);CHKERRQ(ierr);
    if (pep->Dr) {
      ierr = VecPointwiseMult(v,v,pep->Dr);CHKERRQ(ierr);
    }
    ierr = STMatMult(pep->st,0,v,u);CHKERRQ(ierr);
    ierr = VecCopy(t,v);CHKERRQ(ierr);
    if (pep->Dr) {
      ierr = VecPointwiseMult(t,t,pep->Dr);CHKERRQ(ierr);
    }
    ierr = STMatMult(pep->st,1,t,w);CHKERRQ(ierr);
    ierr = VecAXPY(u,pep->sfactor,w);CHKERRQ(ierr);
    ierr = STMatSolve(pep->st,u,w);CHKERRQ(ierr);
    ierr = VecScale(w,-1.0/(pep->sfactor*pep->sfactor));CHKERRQ(ierr);
    if (pep->Dr) {
      ierr = VecPointwiseDivide(w,w,pep->Dr);CHKERRQ(ierr);
    }
    ierr = VecCopy(v,t);CHKERRQ(ierr);
    ierr = BVSetActiveColumns(pep->V,0,j+1);CHKERRQ(ierr);

    /* orthogonalize */
    switch (refinement) {
      case BV_ORTHOG_REFINE_NEVER:
        ierr = PEPQArnoldiCGS(pep,H,ldh,H+ldh*j,j,pep->V,t,v,w,NULL,&norm,work);CHKERRQ(ierr);
        *breakdown = PETSC_FALSE;
        break;
      case BV_ORTHOG_REFINE_ALWAYS:
        ierr = PEPQArnoldiCGS(pep,H,ldh,H+ldh*j,j,pep->V,t,v,w,NULL,NULL,work);CHKERRQ(ierr);
        ierr = PEPQArnoldiCGS(pep,H,ldh,c,j,pep->V,t,v,w,&onorm,&norm,work);CHKERRQ(ierr);
        for (i=0;i<=j;i++) H[ldh*j+i] += c[i];
        if (norm < eta * onorm) *breakdown = PETSC_TRUE;
        else *breakdown = PETSC_FALSE;
        break;
      case BV_ORTHOG_REFINE_IFNEEDED:
        ierr = PEPQArnoldiCGS(pep,H,ldh,H+ldh*j,j,pep->V,t,v,w,&onorm,&norm,work);CHKERRQ(ierr);
        /* ||q|| < eta ||h|| */
        l = 1;
        while (l<3 && norm < eta * onorm) {
          l++;
          onorm = norm;
          ierr = PEPQArnoldiCGS(pep,H,ldh,c,j,pep->V,t,v,w,NULL,&norm,work);CHKERRQ(ierr);
          for (i=0;i<=j;i++) H[ldh*j+i] += c[i];
        }
        if (norm < eta * onorm) *breakdown = PETSC_TRUE;
        else *breakdown = PETSC_FALSE;
        break;
      default: SETERRQ(PetscObjectComm((PetscObject)pep),1,"Wrong value of ip->orth_ref");
    }
    ierr = VecScale(v,1.0/norm);CHKERRQ(ierr);
    ierr = VecScale(w,1.0/norm);CHKERRQ(ierr);

    H[j+1+ldh*j] = norm;
    if (j<m-1) {
      ierr = BVInsertVec(pep->V,j+1,v);CHKERRQ(ierr);
    }
  }
  *beta = norm;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPSolve_QArnoldi"
PetscErrorCode PEPSolve_QArnoldi(PEP pep)
{
  PetscErrorCode ierr;
  PEP_QARNOLDI   *ctx = (PEP_QARNOLDI*)pep->data;
  PetscInt       j,k,l,lwork,nv,ld,newn;
  Vec            v=pep->work[0],w=pep->work[1];
  Mat            Q;
  PetscScalar    *S,*work;
  PetscReal      beta=0.0,norm,x,y;
  PetscBool      breakdown=PETSC_FALSE;

  PetscFunctionBegin;
  ierr = DSGetLeadingDimension(pep->ds,&ld);CHKERRQ(ierr);
  lwork = 7*pep->ncv;
  ierr = PetscMalloc1(lwork,&work);CHKERRQ(ierr);

  /* Modify matrix norms so that the scaling affects the convergence test */
  norm = pep->dsfactor;
  for (j=0;j<pep->nmat;j++) {
    pep->nrma[j] *= norm;
    norm*=pep->sfactor;
  }

  /* Get the starting Arnoldi vector */
  if (pep->nini==0) {
    ierr = BVSetRandomColumn(pep->V,0,pep->rand);CHKERRQ(ierr);
  }
  /* w is always a random vector */
  ierr = BVSetRandomColumn(pep->V,1,pep->rand);CHKERRQ(ierr);
  ierr = BVCopyVec(pep->V,0,v);CHKERRQ(ierr);
  ierr = BVCopyVec(pep->V,1,w);CHKERRQ(ierr);
  ierr = VecNorm(v,NORM_2,&x);CHKERRQ(ierr);
  ierr = VecNorm(w,NORM_2,&y);CHKERRQ(ierr);
  norm = PetscSqrtReal(x*x+y*y);CHKERRQ(ierr);
  ierr = VecScale(v,1.0/norm);CHKERRQ(ierr);
  ierr = VecScale(w,1.0/norm);CHKERRQ(ierr);

   /* Restart loop */
  l = 0;
  while (pep->reason == PEP_CONVERGED_ITERATING) {
    pep->its++;

    /* Compute an nv-step Arnoldi factorization */
    nv = PetscMin(pep->nconv+pep->mpd,pep->ncv);
    ierr = DSGetArray(pep->ds,DS_MAT_A,&S);CHKERRQ(ierr);
    ierr = PEPQArnoldi(pep,S,ld,pep->nconv+l,&nv,v,w,&beta,&breakdown,work);CHKERRQ(ierr);
    ierr = DSRestoreArray(pep->ds,DS_MAT_A,&S);CHKERRQ(ierr);
    ierr = DSSetDimensions(pep->ds,nv,0,pep->nconv,pep->nconv+l);CHKERRQ(ierr);
    if (l==0) {
      ierr = DSSetState(pep->ds,DS_STATE_INTERMEDIATE);CHKERRQ(ierr);
    } else {
      ierr = DSSetState(pep->ds,DS_STATE_RAW);CHKERRQ(ierr);
    }
    ierr = BVSetActiveColumns(pep->V,pep->nconv,nv);CHKERRQ(ierr);

    /* Solve projected problem */
    ierr = DSSolve(pep->ds,pep->eigr,pep->eigi);CHKERRQ(ierr);
    ierr = DSSort(pep->ds,pep->eigr,pep->eigi,NULL,NULL,NULL);CHKERRQ(ierr);
    ierr = DSUpdateExtraRow(pep->ds);CHKERRQ(ierr);

    /* Check convergence */
    ierr = PEPKrylovConvergence(pep,PETSC_FALSE,pep->nconv,nv-pep->nconv,beta,&k);CHKERRQ(ierr);
    if (pep->its >= pep->max_it) pep->reason = PEP_DIVERGED_ITS;
    if (k >= pep->nev) pep->reason = PEP_CONVERGED_TOL;

    /* Update l */
    if (pep->reason != PEP_CONVERGED_ITERATING || breakdown) l = 0;
    else l = PetscMax(1,(PetscInt)((nv-k)*ctx->keep));
    if (!ctx->lock && l>0) { l += k; k = 0; } /* non-locking variant: reset no. of converged pairs */

    if (pep->reason == PEP_CONVERGED_ITERATING) {
      if (breakdown) {
        /* Stop if breakdown */
        ierr = PetscInfo2(pep,"Breakdown Quadratic Arnoldi method (it=%D norm=%g)\n",pep->its,(double)beta);CHKERRQ(ierr);
        pep->reason = PEP_DIVERGED_BREAKDOWN;
      } else {
        /* Prepare the Rayleigh quotient for restart */
        ierr = DSTruncate(pep->ds,k+l);CHKERRQ(ierr);
        ierr = DSGetDimensions(pep->ds,&newn,NULL,NULL,NULL,NULL);CHKERRQ(ierr);
        l = newn-k;
      }
    }
    /* Update the corresponding vectors V(:,idx) = V*Q(:,idx) */
    ierr = DSGetMat(pep->ds,DS_MAT_Q,&Q);CHKERRQ(ierr);
    ierr = BVMultInPlace(pep->V,Q,pep->nconv,k+l);CHKERRQ(ierr);
    ierr = MatDestroy(&Q);CHKERRQ(ierr);

    pep->nconv = k;
    ierr = PEPMonitor(pep,pep->its,pep->nconv,pep->eigr,pep->eigi,pep->errest,nv);CHKERRQ(ierr);
  }

  for (j=0;j<pep->nconv;j++) {
    pep->eigr[j] *= pep->sfactor;
    pep->eigi[j] *= pep->sfactor;
  }

  /* Restore matrix norms */
  norm = pep->dsfactor;
  for (j=0;j<pep->nmat;j++) {
    pep->nrma[j] /= norm;
    norm*=pep->sfactor;
  }

  /* truncate Schur decomposition and change the state to raw so that
     DSVectors() computes eigenvectors from scratch */
  ierr = DSSetDimensions(pep->ds,pep->nconv,0,0,0);CHKERRQ(ierr);
  ierr = DSSetState(pep->ds,DS_STATE_RAW);CHKERRQ(ierr);
  ierr = PetscFree(work);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiSetRestart_QArnoldi"
static PetscErrorCode PEPQArnoldiSetRestart_QArnoldi(PEP pep,PetscReal keep)
{
  PEP_QARNOLDI *ctx = (PEP_QARNOLDI*)pep->data;

  PetscFunctionBegin;
  if (keep==PETSC_DEFAULT) ctx->keep = 0.5;
  else {
    if (keep<0.1 || keep>0.9) SETERRQ(PetscObjectComm((PetscObject)pep),PETSC_ERR_ARG_OUTOFRANGE,"The keep argument must be in the range [0.1,0.9]");
    ctx->keep = keep;
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiSetRestart"
/*@
   PEPQArnoldiSetRestart - Sets the restart parameter for the Q-Arnoldi
   method, in particular the proportion of basis vectors that must be kept
   after restart.

   Logically Collective on PEP

   Input Parameters:
+  pep  - the eigenproblem solver context
-  keep - the number of vectors to be kept at restart

   Options Database Key:
.  -pep_qarnoldi_restart - Sets the restart parameter

   Notes:
   Allowed values are in the range [0.1,0.9]. The default is 0.5.

   Level: advanced

.seealso: PEPQArnoldiGetRestart()
@*/
PetscErrorCode PEPQArnoldiSetRestart(PEP pep,PetscReal keep)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(pep,PEP_CLASSID,1);
  PetscValidLogicalCollectiveReal(pep,keep,2);
  ierr = PetscTryMethod(pep,"PEPQArnoldiSetRestart_C",(PEP,PetscReal),(pep,keep));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiGetRestart_QArnoldi"
static PetscErrorCode PEPQArnoldiGetRestart_QArnoldi(PEP pep,PetscReal *keep)
{
  PEP_QARNOLDI *ctx = (PEP_QARNOLDI*)pep->data;

  PetscFunctionBegin;
  *keep = ctx->keep;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiGetRestart"
/*@
   PEPQArnoldiGetRestart - Gets the restart parameter used in the Q-Arnoldi method.

   Not Collective

   Input Parameter:
.  pep - the eigenproblem solver context

   Output Parameter:
.  keep - the restart parameter

   Level: advanced

.seealso: PEPQArnoldiSetRestart()
@*/
PetscErrorCode PEPQArnoldiGetRestart(PEP pep,PetscReal *keep)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(pep,PEP_CLASSID,1);
  PetscValidPointer(keep,2);
  ierr = PetscTryMethod(pep,"PEPQArnoldiGetRestart_C",(PEP,PetscReal*),(pep,keep));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiSetLocking_QArnoldi"
static PetscErrorCode PEPQArnoldiSetLocking_QArnoldi(PEP pep,PetscBool lock)
{
  PEP_QARNOLDI *ctx = (PEP_QARNOLDI*)pep->data;

  PetscFunctionBegin;
  ctx->lock = lock;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiSetLocking"
/*@
   PEPQArnoldiSetLocking - Choose between locking and non-locking variants of
   the Q-Arnoldi method.

   Logically Collective on PEP

   Input Parameters:
+  pep  - the eigenproblem solver context
-  lock - true if the locking variant must be selected

   Options Database Key:
.  -pep_qarnoldi_locking - Sets the locking flag

   Notes:
   The default is to keep all directions in the working subspace even if
   already converged to working accuracy (the non-locking variant).
   This behaviour can be changed so that converged eigenpairs are locked
   when the method restarts.

   Note that the default behaviour is the opposite to Krylov solvers in EPS.

   Level: advanced

.seealso: PEPQArnoldiGetLocking()
@*/
PetscErrorCode PEPQArnoldiSetLocking(PEP pep,PetscBool lock)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(pep,PEP_CLASSID,1);
  PetscValidLogicalCollectiveBool(pep,lock,2);
  ierr = PetscTryMethod(pep,"PEPQArnoldiSetLocking_C",(PEP,PetscBool),(pep,lock));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiGetLocking_QArnoldi"
static PetscErrorCode PEPQArnoldiGetLocking_QArnoldi(PEP pep,PetscBool *lock)
{
  PEP_QARNOLDI *ctx = (PEP_QARNOLDI*)pep->data;

  PetscFunctionBegin;
  *lock = ctx->lock;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPQArnoldiGetLocking"
/*@
   PEPQArnoldiGetLocking - Gets the locking flag used in the Q-Arnoldi method.

   Not Collective

   Input Parameter:
.  pep - the eigenproblem solver context

   Output Parameter:
.  lock - the locking flag

   Level: advanced

.seealso: PEPQArnoldiSetLocking()
@*/
PetscErrorCode PEPQArnoldiGetLocking(PEP pep,PetscBool *lock)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(pep,PEP_CLASSID,1);
  PetscValidPointer(lock,2);
  ierr = PetscTryMethod(pep,"PEPQArnoldiGetLocking_C",(PEP,PetscBool*),(pep,lock));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPSetFromOptions_QArnoldi"
PetscErrorCode PEPSetFromOptions_QArnoldi(PetscOptions *PetscOptionsObject,PEP pep)
{
  PetscErrorCode ierr;
  PetscBool      flg,lock;
  PetscReal      keep;

  PetscFunctionBegin;
  ierr = PetscOptionsHead(PetscOptionsObject,"PEP Q-Arnoldi Options");CHKERRQ(ierr);
  ierr = PetscOptionsReal("-pep_qarnoldi_restart","Proportion of vectors kept after restart","PEPQArnoldiSetRestart",0.5,&keep,&flg);CHKERRQ(ierr);
  if (flg) {
    ierr = PEPQArnoldiSetRestart(pep,keep);CHKERRQ(ierr);
  }
  ierr = PetscOptionsBool("-pep_qarnoldi_locking","Choose between locking and non-locking variants","PEPQArnoldiSetLocking",PETSC_TRUE,&lock,&flg);CHKERRQ(ierr);
  if (flg) {
    ierr = PEPQArnoldiSetLocking(pep,lock);CHKERRQ(ierr);
  }
  ierr = PetscOptionsTail();CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPView_QArnoldi"
PetscErrorCode PEPView_QArnoldi(PEP pep,PetscViewer viewer)
{
  PetscErrorCode ierr;
  PEP_QARNOLDI   *ctx = (PEP_QARNOLDI*)pep->data;
  PetscBool      isascii;

  PetscFunctionBegin;
  ierr = PetscObjectTypeCompare((PetscObject)viewer,PETSCVIEWERASCII,&isascii);CHKERRQ(ierr);
  if (isascii) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Q-Arnoldi: %d%% of basis vectors kept after restart\n",(int)(100*ctx->keep));CHKERRQ(ierr);
    ierr = PetscViewerASCIIPrintf(viewer,"  Q-Arnoldi: using the %slocking variant\n",ctx->lock?"":"non-");CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPDestroy_QArnoldi"
PetscErrorCode PEPDestroy_QArnoldi(PEP pep)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscFree(pep->data);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiSetRestart_C",NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiGetRestart_C",NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiSetLocking_C",NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiGetLocking_C",NULL);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PEPCreate_QArnoldi"
PETSC_EXTERN PetscErrorCode PEPCreate_QArnoldi(PEP pep)
{
  PEP_QARNOLDI   *ctx;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscNewLog(pep,&ctx);CHKERRQ(ierr);
  pep->data = (void*)ctx;
  ctx->lock = PETSC_FALSE;

  pep->ops->solve          = PEPSolve_QArnoldi;
  pep->ops->setup          = PEPSetUp_QArnoldi;
  pep->ops->setfromoptions = PEPSetFromOptions_QArnoldi;
  pep->ops->destroy        = PEPDestroy_QArnoldi;
  pep->ops->view           = PEPView_QArnoldi;
  pep->ops->computevectors = PEPComputeVectors_Schur;
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiSetRestart_C",PEPQArnoldiSetRestart_QArnoldi);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiGetRestart_C",PEPQArnoldiGetRestart_QArnoldi);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiSetLocking_C",PEPQArnoldiSetLocking_QArnoldi);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunction((PetscObject)pep,"PEPQArnoldiGetLocking_C",PEPQArnoldiGetLocking_QArnoldi);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

