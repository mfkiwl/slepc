/*                       

   SLEPc eigensolver: "arnoldi"

   Method: Explicitly Restarted Arnoldi

   Algorithm:

       Arnoldi method with explicit restart and deflation.

   References:

       [1] "Arnoldi Methods in SLEPc", SLEPc Technical Report STR-4, 
           available at http://www.grycap.upv.es/slepc.

   Last update: Feb 2009

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2011, Universitat Politecnica de Valencia, Spain

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

#include <slepc-private/epsimpl.h>                /*I "slepceps.h" I*/
#include <slepcblaslapack.h>

PetscErrorCode EPSSolve_Arnoldi(EPS);

typedef struct {
  PetscBool delayed;
} EPS_ARNOLDI;

#undef __FUNCT__  
#define __FUNCT__ "EPSSetUp_Arnoldi"
PetscErrorCode EPSSetUp_Arnoldi(EPS eps)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (eps->ncv) { /* ncv set */
    if (eps->ncv<eps->nev) SETERRQ(((PetscObject)eps)->comm,1,"The value of ncv must be at least nev"); 
  }
  else if (eps->mpd) { /* mpd set */
    eps->ncv = PetscMin(eps->n,eps->nev+eps->mpd);
  }
  else { /* neither set: defaults depend on nev being small or large */
    if (eps->nev<500) eps->ncv = PetscMin(eps->n,PetscMax(2*eps->nev,eps->nev+15));
    else { eps->mpd = 500; eps->ncv = PetscMin(eps->n,eps->nev+eps->mpd); }
  }
  if (!eps->mpd) eps->mpd = eps->ncv;
  if (eps->ncv>eps->nev+eps->mpd) SETERRQ(((PetscObject)eps)->comm,1,"The value of ncv must not be larger than nev+mpd"); 
  if (!eps->max_it) eps->max_it = PetscMax(100,2*eps->n/eps->ncv);
  if (!eps->which) { ierr = EPSDefaultSetWhich(eps);CHKERRQ(ierr); }
  if (eps->ishermitian && (eps->which==EPS_LARGEST_IMAGINARY || eps->which==EPS_SMALLEST_IMAGINARY)) SETERRQ(((PetscObject)eps)->comm,1,"Wrong value of eps->which");

  if (!eps->extraction) {
    ierr = EPSSetExtraction(eps,EPS_RITZ);CHKERRQ(ierr);
  }

  ierr = EPSAllocateSolution(eps);CHKERRQ(ierr);
  ierr = PSSetType(eps->ps,PSNHEP);CHKERRQ(ierr);
  ierr = PSAllocate(eps->ps,eps->ncv);CHKERRQ(ierr);
  ierr = EPSDefaultGetWork(eps,1);CHKERRQ(ierr);

  /* dispatch solve method */
  if (eps->leftvecs) SETERRQ(((PetscObject)eps)->comm,PETSC_ERR_SUP,"Left vectors not supported in this solver");
  eps->ops->solve = EPSSolve_Arnoldi;
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSDelayedArnoldi"
/*
   EPSDelayedArnoldi - This function is equivalent to EPSBasicArnoldi but
   performs the computation in a different way. The main idea is that
   reorthogonalization is delayed to the next Arnoldi step. This version is
   more scalable but in some cases convergence may stagnate.
*/
PetscErrorCode EPSDelayedArnoldi(EPS eps,PetscScalar *H,PetscInt ldh,Vec *V,PetscInt k,PetscInt *M,Vec f,PetscReal *beta,PetscBool *breakdown)
{
  PetscErrorCode ierr;
  PetscInt       i,j,m=*M;
  Vec            u,t;
  PetscScalar    shh[100],*lhh,dot,dot2;
  PetscReal      norm1=0.0,norm2;

  PetscFunctionBegin;
  if (m<=100) lhh = shh;
  else { ierr = PetscMalloc(m*sizeof(PetscScalar),&lhh);CHKERRQ(ierr); }
  ierr = VecDuplicate(f,&u);CHKERRQ(ierr);
  ierr = VecDuplicate(f,&t);CHKERRQ(ierr);

  for (j=k;j<m;j++) {
    ierr = STApply(eps->OP,V[j],f);CHKERRQ(ierr);
    ierr = IPOrthogonalize(eps->ip,0,PETSC_NULL,eps->nds,PETSC_NULL,eps->DS,f,PETSC_NULL,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);

    ierr = IPMInnerProductBegin(eps->ip,f,j+1,V,H+ldh*j);CHKERRQ(ierr);
    if (j>k) { 
      ierr = IPMInnerProductBegin(eps->ip,V[j],j,V,lhh);CHKERRQ(ierr);
      ierr = IPInnerProductBegin(eps->ip,V[j],V[j],&dot);CHKERRQ(ierr); 
    }
    if (j>k+1) {
      ierr = IPNormBegin(eps->ip,u,&norm2);CHKERRQ(ierr); 
      ierr = VecDotBegin(u,V[j-2],&dot2);CHKERRQ(ierr);
    }
    
    ierr = IPMInnerProductEnd(eps->ip,f,j+1,V,H+ldh*j);CHKERRQ(ierr);
    if (j>k) { 
      ierr = IPMInnerProductEnd(eps->ip,V[j],j,V,lhh);CHKERRQ(ierr);
      ierr = IPInnerProductEnd(eps->ip,V[j],V[j],&dot);CHKERRQ(ierr); 
    }
    if (j>k+1) {
      ierr = IPNormEnd(eps->ip,u,&norm2);CHKERRQ(ierr);
      ierr = VecDotEnd(u,V[j-2],&dot2);CHKERRQ(ierr);
      if (PetscAbsScalar(dot2/norm2) > PETSC_MACHINE_EPSILON) {
        *breakdown = PETSC_TRUE;
        *M = j-1;
        *beta = norm2;

        if (m>100) { ierr = PetscFree(lhh);CHKERRQ(ierr); }
        ierr = VecDestroy(&u);CHKERRQ(ierr);
        ierr = VecDestroy(&t);CHKERRQ(ierr);
        PetscFunctionReturn(0);
      }
    }
    
    if (j>k) {      
      norm1 = PetscSqrtReal(PetscRealPart(dot));
      for (i=0;i<j;i++)
        H[ldh*j+i] = H[ldh*j+i]/norm1;
      H[ldh*j+j] = H[ldh*j+j]/dot;
      
      ierr = VecCopy(V[j],t);CHKERRQ(ierr);
      ierr = VecScale(V[j],1.0/norm1);CHKERRQ(ierr);
      ierr = VecScale(f,1.0/norm1);CHKERRQ(ierr);
    }

    ierr = SlepcVecMAXPBY(f,1.0,-1.0,j+1,H+ldh*j,V);CHKERRQ(ierr);

    if (j>k) {
      ierr = SlepcVecMAXPBY(t,1.0,-1.0,j,lhh,V);CHKERRQ(ierr);
      for (i=0;i<j;i++)
        H[ldh*(j-1)+i] += lhh[i];
    }

    if (j>k+1) {
      ierr = VecCopy(u,V[j-1]);CHKERRQ(ierr);
      ierr = VecScale(V[j-1],1.0/norm2);CHKERRQ(ierr);
      H[ldh*(j-2)+j-1] = norm2;
    }

    if (j<m-1) {
      ierr = VecCopy(f,V[j+1]);CHKERRQ(ierr);
      ierr = VecCopy(t,u);CHKERRQ(ierr);
    }
  }

  ierr = IPNorm(eps->ip,t,&norm2);CHKERRQ(ierr);
  ierr = VecScale(t,1.0/norm2);CHKERRQ(ierr);
  ierr = VecCopy(t,V[m-1]);CHKERRQ(ierr);
  H[ldh*(m-2)+m-1] = norm2;

  ierr = IPMInnerProduct(eps->ip,f,m,V,lhh);CHKERRQ(ierr);
  
  ierr = SlepcVecMAXPBY(f,1.0,-1.0,m,lhh,V);CHKERRQ(ierr);
  for (i=0;i<m;i++)
    H[ldh*(m-1)+i] += lhh[i];

  ierr = IPNorm(eps->ip,f,beta);CHKERRQ(ierr);
  ierr = VecScale(f,1.0 / *beta);CHKERRQ(ierr);
  *breakdown = PETSC_FALSE;
  
  if (m>100) { ierr = PetscFree(lhh);CHKERRQ(ierr); }
  ierr = VecDestroy(&u);CHKERRQ(ierr);
  ierr = VecDestroy(&t);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSDelayedArnoldi1"
/*
   EPSDelayedArnoldi1 - This function is similar to EPSDelayedArnoldi1,
   but without reorthogonalization (only delayed normalization).
*/
PetscErrorCode EPSDelayedArnoldi1(EPS eps,PetscScalar *H,PetscInt ldh,Vec *V,PetscInt k,PetscInt *M,Vec f,PetscReal *beta,PetscBool *breakdown)
{
  PetscErrorCode ierr;
  PetscInt       i,j,m=*M;
  PetscScalar    dot;
  PetscReal      norm=0.0;

  PetscFunctionBegin;
  for (j=k;j<m;j++) {
    ierr = STApply(eps->OP,V[j],f);CHKERRQ(ierr);
    ierr = IPOrthogonalize(eps->ip,0,PETSC_NULL,eps->nds,PETSC_NULL,eps->DS,f,PETSC_NULL,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);

    ierr = IPMInnerProductBegin(eps->ip,f,j+1,V,H+ldh*j);CHKERRQ(ierr);
    if (j>k) { 
      ierr = IPInnerProductBegin(eps->ip,V[j],V[j],&dot);CHKERRQ(ierr); 
    }
    
    ierr = IPMInnerProductEnd(eps->ip,f,j+1,V,H+ldh*j);CHKERRQ(ierr);
    if (j>k) { 
      ierr = IPInnerProductEnd(eps->ip,V[j],V[j],&dot);CHKERRQ(ierr); 
    }
    
    if (j>k) {      
      norm = PetscSqrtReal(PetscRealPart(dot));
      ierr = VecScale(V[j],1.0/norm);CHKERRQ(ierr);
      H[ldh*(j-1)+j] = norm;

      for (i=0;i<j;i++)
        H[ldh*j+i] = H[ldh*j+i]/norm;
      H[ldh*j+j] = H[ldh*j+j]/dot;      
      ierr = VecScale(f,1.0/norm);CHKERRQ(ierr);
    }

    ierr = SlepcVecMAXPBY(f,1.0,-1.0,j+1,H+ldh*j,V);CHKERRQ(ierr);

    if (j<m-1) {
      ierr = VecCopy(f,V[j+1]);CHKERRQ(ierr);
    }
  }

  ierr = IPNorm(eps->ip,f,beta);CHKERRQ(ierr);
  ierr = VecScale(f,1.0 / *beta);CHKERRQ(ierr);
  *breakdown = PETSC_FALSE;
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSUpdateVectors"
/*
   EPSUpdateVectors - Computes approximate Schur vectors (or eigenvectors) by
   either Ritz extraction (U=U*Q) or refined Ritz extraction 

   On input:
     n is the size of U
     U is the orthogonal basis of the subspace used for projecting
     s is the index of the first vector computed
     e+1 is the index of the last vector computed
     Q contains the corresponding Schur vectors of the projected matrix (size n x n, leading dimension ldq)
     H is the (extended) projected matrix (size n+1 x n, leading dimension ldh)

   On output:
     v is the resulting vector
*/
PetscErrorCode EPSUpdateVectors(EPS eps,PetscInt n_,Vec *U,PetscInt s,PetscInt e,PetscScalar *Q,PetscInt ldq,PetscScalar *H,PetscInt ldh_)
{
#if defined(PETSC_MISSING_LAPACK_GESVD) 
  PetscFunctionBegin;
  SETERRQ(PETSC_COMM_SELF,PETSC_ERR_SUP,"GESVD - Lapack routine is unavailable.");
#else
  PetscErrorCode ierr;
  PetscBool      refined;
  PetscInt       i,j,k;
  PetscBLASInt   n1,lwork,idummy=1,info,n=n_,ldh=ldh_;
  PetscScalar    *B,sdummy,*work;
  PetscReal      *sigma;

  PetscFunctionBegin;
  refined = (eps->extraction==EPS_REFINED || eps->extraction==EPS_REFINED_HARMONIC)?PETSC_TRUE:PETSC_FALSE;
  if (refined) {
    /* Refined Ritz extraction */
    n1 = n+1;
    ierr = PetscMalloc(n1*n*sizeof(PetscScalar),&B);CHKERRQ(ierr);
    ierr = PetscMalloc(6*n*sizeof(PetscReal),&sigma);CHKERRQ(ierr);
    lwork = 10*n;
    ierr = PetscMalloc(lwork*sizeof(PetscScalar),&work);CHKERRQ(ierr);
    
    for (k=s;k<e;k++) {
      /* copy H to B */
      for (i=0;i<=n;i++) {
        for (j=0;j<n;j++) {
          B[i+j*n1] = H[i+j*ldh];
        }
      }
      /* subtract ritz value from diagonal of B^ */
      for (i=0;i<n;i++) {
        B[i+i*n1] -= eps->eigr[k];  /* MISSING: complex case */
      }
      /* compute SVD of [H-mu*I] */
      ierr = PetscFPTrapPush(PETSC_FP_TRAP_OFF);CHKERRQ(ierr);
#if !defined(PETSC_USE_COMPLEX)
      LAPACKgesvd_("N","O",&n1,&n,B,&n1,sigma,&sdummy,&idummy,&sdummy,&idummy,work,&lwork,&info);
#else
      LAPACKgesvd_("N","O",&n1,&n,B,&n1,sigma,&sdummy,&idummy,&sdummy,&idummy,work,&lwork,sigma+n,&info);
#endif
      ierr = PetscFPTrapPop();CHKERRQ(ierr);
      if (info) SETERRQ1(((PetscObject)eps)->comm,PETSC_ERR_LIB,"Error in Lapack xGESVD %d",info);
      /* the smallest singular value is the new error estimate */
      eps->errest[k] = sigma[n-1];
      /* update vector with right singular vector associated to smallest singular value */
      for (i=0;i<n;i++)
        Q[k*ldq+i] = B[n-1+i*n1];
    }
    /* free workspace */
    ierr = PetscFree(B);CHKERRQ(ierr);
    ierr = PetscFree(sigma);CHKERRQ(ierr);
    ierr = PetscFree(work);CHKERRQ(ierr);
  }
  /* Ritz extraction: v = U*q */
  ierr = SlepcUpdateVectors(n_,U,s,e,Q,ldq,PETSC_FALSE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
#endif
}

#undef __FUNCT__  
#define __FUNCT__ "EPSSolve_Arnoldi"
PetscErrorCode EPSSolve_Arnoldi(EPS eps)
{
  PetscErrorCode     ierr;
  PetscInt           i,k,nv,ld;
  Vec                f=eps->work[0];
  PetscScalar        *H,*U,*Hcopy;
  PetscReal          beta,gamma=1.0;
  PetscBool          breakdown,harmonic,refined;
  IPOrthogRefineType orthog_ref;
  EPS_ARNOLDI        *arnoldi = (EPS_ARNOLDI*)eps->data;

  PetscFunctionBegin;
  ierr = PSGetLeadingDimension(eps->ps,&ld);CHKERRQ(ierr);
  harmonic = (eps->extraction==EPS_HARMONIC || eps->extraction==EPS_REFINED_HARMONIC)?PETSC_TRUE:PETSC_FALSE;
  refined = (eps->extraction==EPS_REFINED || eps->extraction==EPS_REFINED_HARMONIC)?PETSC_TRUE:PETSC_FALSE;
  if (refined) { ierr = PetscMalloc((ld+1)*ld*sizeof(PetscScalar),&Hcopy);CHKERRQ(ierr); }
  
  ierr = IPGetOrthogonalization(eps->ip,PETSC_NULL,&orthog_ref,PETSC_NULL);CHKERRQ(ierr);

  /* Get the starting Arnoldi vector */
  ierr = EPSGetStartVector(eps,0,eps->V[0],PETSC_NULL);CHKERRQ(ierr);
  
  /* Restart loop */
  while (eps->reason == EPS_CONVERGED_ITERATING) {
    eps->its++;

    /* Compute an nv-step Arnoldi factorization */
    nv = PetscMin(eps->nconv+eps->mpd,eps->ncv);
    ierr = PSSetDimensions(eps->ps,nv,eps->nconv,0);CHKERRQ(ierr);
    ierr = PSGetArray(eps->ps,PS_MAT_A,&H);CHKERRQ(ierr);
    if (!arnoldi->delayed) {
      ierr = EPSBasicArnoldi(eps,PETSC_FALSE,H,ld,eps->V,eps->nconv,&nv,f,&beta,&breakdown);CHKERRQ(ierr);
    } else if (orthog_ref == IP_ORTHOG_REFINE_NEVER) {
      ierr = EPSDelayedArnoldi1(eps,H,ld,eps->V,eps->nconv,&nv,f,&beta,&breakdown);CHKERRQ(ierr);
    } else {
      ierr = EPSDelayedArnoldi(eps,H,ld,eps->V,eps->nconv,&nv,f,&beta,&breakdown);CHKERRQ(ierr);
    }
    if (refined) {
      ierr = PetscMemcpy(Hcopy,H,ld*ld*sizeof(PetscScalar));CHKERRQ(ierr);
      for (i=0;i<nv-1;i++) Hcopy[nv+i*ld] = 0.0; 
      Hcopy[nv+(nv-1)*ld] = beta;
    }
    ierr = PSRestoreArray(eps->ps,PS_MAT_A,&H);CHKERRQ(ierr);
    ierr = PSSetState(eps->ps,PS_STATE_INTERMEDIATE);CHKERRQ(ierr);

    /* Compute translation of Krylov decomposition if harmonic extraction used */ 
    if (harmonic) {
      ierr = PSTranslateHarmonic(eps->ps,eps->target,beta,PETSC_FALSE,PETSC_NULL,&gamma);CHKERRQ(ierr);
    }

    /* Solve projected problem */ 
    ierr = PSSolve(eps->ps,eps->eigr,eps->eigi);CHKERRQ(ierr);
    ierr = PSSort(eps->ps,eps->eigr,eps->eigi,eps->which_func,eps->which_ctx);CHKERRQ(ierr);

    /* Check convergence */ 
    ierr = PSGetArray(eps->ps,PS_MAT_A,&H);CHKERRQ(ierr);
    ierr = PSGetArray(eps->ps,PS_MAT_Q,&U);CHKERRQ(ierr);
    ierr = EPSKrylovConvergence(eps,PETSC_FALSE,eps->nconv,nv-eps->nconv,eps->V,nv,beta,gamma,&k);CHKERRQ(ierr);

    ierr = EPSUpdateVectors(eps,nv,eps->V,eps->nconv,PetscMin(k+1,nv),U,ld,Hcopy,ld);CHKERRQ(ierr);
    ierr = PSRestoreArray(eps->ps,PS_MAT_A,&H);CHKERRQ(ierr);
    ierr = PSRestoreArray(eps->ps,PS_MAT_Q,&U);CHKERRQ(ierr);
    eps->nconv = k;

    ierr = EPSMonitor(eps,eps->its,eps->nconv,eps->eigr,eps->eigi,eps->errest,nv);CHKERRQ(ierr);
    if (breakdown) {
      ierr = PetscInfo2(eps,"Breakdown in Arnoldi method (it=%D norm=%G)\n",eps->its,beta);CHKERRQ(ierr);
      ierr = EPSGetStartVector(eps,k,eps->V[k],&breakdown);CHKERRQ(ierr);
      if (breakdown) {
        eps->reason = EPS_DIVERGED_BREAKDOWN;
        ierr = PetscInfo(eps,"Unable to generate more start vectors\n");CHKERRQ(ierr);
      }
    }
    if (eps->its >= eps->max_it) eps->reason = EPS_DIVERGED_ITS;
    if (eps->nconv >= eps->nev) eps->reason = EPS_CONVERGED_TOL;
  }
  
  if (refined) { ierr = PetscFree(Hcopy);CHKERRQ(ierr); }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSSetFromOptions_Arnoldi"
PetscErrorCode EPSSetFromOptions_Arnoldi(EPS eps)
{
  PetscErrorCode ierr;
  PetscBool      set,val;
  EPS_ARNOLDI    *arnoldi = (EPS_ARNOLDI*)eps->data;

  PetscFunctionBegin;
  ierr = PetscOptionsHead("EPS Arnoldi Options");CHKERRQ(ierr);
  ierr = PetscOptionsBool("-eps_arnoldi_delayed","Arnoldi with delayed reorthogonalization","EPSArnoldiSetDelayed",arnoldi->delayed,&val,&set);CHKERRQ(ierr);
  if (set) {
    ierr = EPSArnoldiSetDelayed(eps,val);CHKERRQ(ierr);
  }
  ierr = PetscOptionsTail();CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "EPSArnoldiSetDelayed_Arnoldi"
PetscErrorCode EPSArnoldiSetDelayed_Arnoldi(EPS eps,PetscBool delayed)
{
  EPS_ARNOLDI *arnoldi = (EPS_ARNOLDI*)eps->data;

  PetscFunctionBegin;
  arnoldi->delayed = delayed;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__  
#define __FUNCT__ "EPSArnoldiSetDelayed"
/*@
   EPSArnoldiSetDelayed - Activates or deactivates delayed reorthogonalization 
   in the Arnoldi iteration. 

   Logically Collective on EPS

   Input Parameters:
+  eps - the eigenproblem solver context
-  delayed - boolean flag

   Options Database Key:
.  -eps_arnoldi_delayed - Activates delayed reorthogonalization in Arnoldi
   
   Note:
   Delayed reorthogonalization is an aggressive optimization for the Arnoldi
   eigensolver than may provide better scalability, but sometimes makes the
   solver converge less than the default algorithm.

   Level: advanced

.seealso: EPSArnoldiGetDelayed()
@*/
PetscErrorCode EPSArnoldiSetDelayed(EPS eps,PetscBool delayed)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_CLASSID,1);
  PetscValidLogicalCollectiveBool(eps,delayed,2);
  ierr = PetscTryMethod(eps,"EPSArnoldiSetDelayed_C",(EPS,PetscBool),(eps,delayed));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "EPSArnoldiGetDelayed_Arnoldi"
PetscErrorCode EPSArnoldiGetDelayed_Arnoldi(EPS eps,PetscBool *delayed)
{
  EPS_ARNOLDI *arnoldi = (EPS_ARNOLDI*)eps->data;

  PetscFunctionBegin;
  *delayed = arnoldi->delayed;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNCT__  
#define __FUNCT__ "EPSArnoldiGetDelayed"
/*@C
   EPSArnoldiGetDelayed - Gets the type of reorthogonalization used during the Arnoldi
   iteration. 

   Not Collective

   Input Parameter:
.  eps - the eigenproblem solver context

   Input Parameter:
.  delayed - boolean flag indicating if delayed reorthogonalization has been enabled

   Level: advanced

.seealso: EPSArnoldiSetDelayed()
@*/
PetscErrorCode EPSArnoldiGetDelayed(EPS eps,PetscBool *delayed)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(eps,EPS_CLASSID,1);
  PetscValidPointer(delayed,2);
  ierr = PetscTryMethod(eps,"EPSArnoldiGetDelayed_C",(EPS,PetscBool*),(eps,delayed));CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSDestroy_Arnoldi"
PetscErrorCode EPSDestroy_Arnoldi(EPS eps)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscFree(eps->data);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSArnoldiSetDelayed_C","",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSArnoldiGetDelayed_C","",PETSC_NULL);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSView_Arnoldi"
PetscErrorCode EPSView_Arnoldi(EPS eps,PetscViewer viewer)
{
  PetscErrorCode ierr;
  PetscBool      isascii;
  EPS_ARNOLDI    *arnoldi = (EPS_ARNOLDI*)eps->data;

  PetscFunctionBegin;
  ierr = PetscTypeCompare((PetscObject)viewer,PETSCVIEWERASCII,&isascii);CHKERRQ(ierr);
  if (!isascii) SETERRQ1(((PetscObject)eps)->comm,1,"Viewer type %s not supported for EPS Arnoldi",((PetscObject)viewer)->type_name);
  if (arnoldi->delayed) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Arnoldi: using delayed reorthogonalization\n");CHKERRQ(ierr);
  }  
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "EPSCreate_Arnoldi"
PetscErrorCode EPSCreate_Arnoldi(EPS eps)
{
  PetscErrorCode ierr;
  
  PetscFunctionBegin;
  ierr = PetscNewLog(eps,EPS_ARNOLDI,&eps->data);CHKERRQ(ierr);
  eps->ops->setup                = EPSSetUp_Arnoldi;
  eps->ops->setfromoptions       = EPSSetFromOptions_Arnoldi;
  eps->ops->destroy              = EPSDestroy_Arnoldi;
  eps->ops->reset                = EPSReset_Default;
  eps->ops->view                 = EPSView_Arnoldi;
  eps->ops->backtransform        = EPSBackTransform_Default;
  eps->ops->computevectors       = EPSComputeVectors_Schur;
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSArnoldiSetDelayed_C","EPSArnoldiSetDelayed_Arnoldi",EPSArnoldiSetDelayed_Arnoldi);CHKERRQ(ierr);
  ierr = PetscObjectComposeFunctionDynamic((PetscObject)eps,"EPSArnoldiGetDelayed_C","EPSArnoldiGetDelayed_Arnoldi",EPSArnoldiGetDelayed_Arnoldi);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
EXTERN_C_END

