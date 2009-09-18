/*                       

   SLEPc eigensolver: "lanczos"

   Method: Explicitly Restarted Non-Symmetric Lanczos 

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

#undef __FUNCT__  
#define __FUNCT__ "EPSPlainBiLanczos"
static PetscErrorCode EPSPlainBiLanczos(EPS eps,PetscScalar *T,PetscScalar *Tl,Vec *V,Vec *W,PetscScalar *delta,PetscInt k,PetscInt *M,Vec fv,Vec fw,PetscReal *betav,PetscReal *betaw)
{
  PetscErrorCode ierr;
  PetscInt       i,j,m = *M;
  PetscReal      norm;
  PetscScalar    coef;
  //PetscTruth     breakdown = PETSC_FALSE;

#define alpha(j)  T[j+j*eps->ncv]
#define beta(j)   T[j-1+j*eps->ncv]
#define rho(j)    T[j+(j-1)*eps->ncv]
#define alphal(j) Tl[j+j*eps->ncv]
#define gamma(j)  Tl[j-1+j*eps->ncv]
#define eta(j)    Tl[j+(j-1)*eps->ncv]

  PetscFunctionBegin;

  /*ierr = STNorm(eps->OP,V[k],&norm);CHKERRQ(ierr);  
  ierr = VecScale(V[k],1.0/norm);CHKERRQ(ierr);
  ierr = STNorm(eps->OP,W[k],&norm);CHKERRQ(ierr);  
  ierr = VecScale(W[k],1.0/norm);CHKERRQ(ierr);*/

  for (j=k;j<m-1;j++) {

    ierr = IPInnerProduct(eps->ip,V[j],W[j],&delta[j]);CHKERRQ(ierr);

    ierr = STApply(eps->OP,V[j],V[j+1]);CHKERRQ(ierr); 
    // deflation
    for (i=0;i<k;i++) {
      ierr = IPInnerProduct(eps->ip,V[j+1],W[i],&coef);CHKERRQ(ierr);
      ierr = VecAXPY(V[j+1],-coef/delta[i],V[i]);CHKERRQ(ierr);
    }
    ierr = IPInnerProduct(eps->ip,V[j+1],W[j],&alpha(j));CHKERRQ(ierr);
    alpha(j) = alpha(j)/delta[j]; 
    alphal(j) = alpha(j); 
    if (j>k) {
      beta(j) = delta[j]/delta[j-1]*eta(j);
      gamma(j) = delta[j]/delta[j-1]*rho(j);
    }

    ierr = VecAXPY(V[j+1],-alpha(j),V[j]);CHKERRQ(ierr);
    if (j>k) { ierr = VecAXPY(V[j+1],-beta(j),V[j-1]);CHKERRQ(ierr); }
    // re-orthogonalization
    for (i=0;i<=j;i++) {
      ierr = IPInnerProduct(eps->ip,V[j+1],W[i],&coef);CHKERRQ(ierr);
      ierr = VecAXPY(V[j+1],-coef/delta[i],V[i]);CHKERRQ(ierr);
    }
    ierr = IPNorm(eps->ip,V[j+1],&norm);CHKERRQ(ierr);  
    rho(j+1) = norm;
    ierr = VecScale(V[j+1],1.0/norm);CHKERRQ(ierr);

    ierr = STApplyTranspose(eps->OP,W[j],W[j+1]);CHKERRQ(ierr); 
    // deflation
    for (i=0;i<k;i++) {
      ierr = IPInnerProduct(eps->ip,W[j+1],V[i],&coef);CHKERRQ(ierr);
      ierr = VecAXPY(W[j+1],-coef/delta[i],W[i]);CHKERRQ(ierr);
    }

    ierr = VecAXPY(W[j+1],-alpha(j),W[j]);CHKERRQ(ierr);
    if (j>k) { ierr = VecAXPY(W[j+1],-gamma(j),W[j-1]);CHKERRQ(ierr); }
    // re-orthogonalization
    for (i=0;i<=j;i++) {
      ierr = IPInnerProduct(eps->ip,W[j+1],V[i],&coef);CHKERRQ(ierr);
      ierr = VecAXPY(W[j+1],-coef/delta[i],W[i]);CHKERRQ(ierr);
    }
    ierr = IPNorm(eps->ip,W[j+1],&norm);CHKERRQ(ierr);  
    eta(j+1) = norm;
    ierr = VecScale(W[j+1],1.0/norm);CHKERRQ(ierr);

    eps->its++;
  }

  // j=m-1
  ierr = IPInnerProduct(eps->ip,V[j],W[j],&delta[j]);CHKERRQ(ierr);

  ierr = STApply(eps->OP,V[j],fv);CHKERRQ(ierr); 
  // deflation
  for (i=0;i<k;i++) {
    ierr = IPInnerProduct(eps->ip,fv,W[i],&coef);CHKERRQ(ierr);
    ierr = VecAXPY(fv,-coef/delta[i],V[i]);CHKERRQ(ierr);
  }
  ierr = IPInnerProduct(eps->ip,fv,W[j],&alpha(j));CHKERRQ(ierr);
  alpha(j) = alpha(j)/delta[j]; 
  alphal(j) = alpha(j); 
  beta(j) = delta[j]/delta[j-1]*eta(j);
  gamma(j) = delta[j]/delta[j-1]*rho(j);

  ierr = VecAXPY(fv,-alpha(j),V[j]);CHKERRQ(ierr);
  ierr = VecAXPY(fv,-beta(j),V[j-1]);CHKERRQ(ierr);
  // re-orthogonalization
  for (i=0;i<=j;i++) {
    ierr = IPInnerProduct(eps->ip,fv,W[i],&coef);CHKERRQ(ierr);
    ierr = VecAXPY(fv,-coef/delta[i],V[i]);CHKERRQ(ierr);
  }
  ierr = IPNorm(eps->ip,fv,betav);CHKERRQ(ierr);  

  ierr = STApplyTranspose(eps->OP,W[j],fw);CHKERRQ(ierr); 
  // deflation
  for (i=0;i<k;i++) {
    ierr = IPInnerProduct(eps->ip,fw,V[i],&coef);CHKERRQ(ierr);
    ierr = VecAXPY(fw,-coef/delta[i],W[i]);CHKERRQ(ierr);
  }

  ierr = VecAXPY(fw,-alpha(j),W[j]);CHKERRQ(ierr);
  ierr = VecAXPY(fw,-gamma(j),W[j-1]);CHKERRQ(ierr);
  // re-orthogonalization
  for (i=0;i<=j;i++) {
    ierr = IPInnerProduct(eps->ip,fw,V[i],&coef);CHKERRQ(ierr);
    ierr = VecAXPY(fw,-coef/delta[i],W[i]);CHKERRQ(ierr);
  }
  ierr = IPNorm(eps->ip,fw,betaw);CHKERRQ(ierr);  

  eps->its++;
  
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "EPSSolve_TS_LANCZOS"
PetscErrorCode EPSSolve_TS_LANCZOS(EPS eps)
{
  PetscErrorCode ierr;
  PetscInt       i,j,k,m,N,*perm,
                 ncv=eps->ncv;
  Vec            fv=eps->work[0];
  Vec            fw=eps->work[1];
  PetscScalar    *T=eps->T,*Tl=eps->Tl,/* *Y, */ *U,*Ul,*delta,*eigr,*eigi,*work;
  PetscReal      *ritz,*bnd,*E,betav,betaw;

  PetscFunctionBegin;
  ierr = PetscMalloc(ncv*sizeof(PetscReal),&ritz);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscReal),&bnd);CHKERRQ(ierr);
  //ierr = PetscMalloc(ncv*ncv*sizeof(PetscScalar),&Y);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*ncv*sizeof(PetscScalar),&U);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*ncv*sizeof(PetscScalar),&Ul);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscReal),&E);CHKERRQ(ierr);
  ierr = PetscMalloc((ncv+4)*ncv*sizeof(PetscScalar),&work);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscInt),&perm);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscScalar),&delta);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscScalar),&eigr);CHKERRQ(ierr);
  ierr = PetscMalloc(ncv*sizeof(PetscScalar),&eigi);CHKERRQ(ierr);

  /* The first Lanczos vector is the normalized initial vector */
  ierr = EPSGetStartVector(eps,0,eps->V[0],PETSC_NULL);CHKERRQ(ierr);
  ierr = EPSGetLeftStartVector(eps,0,eps->W[0]);CHKERRQ(ierr);
  
  eps->nconv = 0;
  eps->its = 0;
  for (i=0;i<eps->ncv;i++) eps->eigr[i]=eps->eigi[i]=eps->errest[i]=0.0;
  EPSMonitor(eps,eps->its,eps->nconv,eps->eigr,eps->eigi,eps->errest,ncv);
  ierr = VecGetSize(eps->vec_initial,&N);CHKERRQ(ierr);
  
  /* Restart loop */
  while (eps->its<eps->max_it) {
    /* Compute an ncv-step Lanczos factorization */
    m = ncv;
    ierr = EPSPlainBiLanczos(eps,T,Tl,eps->V,eps->W,delta,eps->nconv,&m,fv,fw,&betav,&betaw);CHKERRQ(ierr);
//    ierr = SlepcCheckOrthogonality(eps->V,4,eps->W,4,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);

    /* Reduce T to (quasi-)triangular form, T <- U T U' */
    ierr = PetscMemzero(U,ncv*ncv*sizeof(PetscScalar));CHKERRQ(ierr);
    for (i=0;i<ncv;i++) { U[i*(ncv+1)] = 1.0; }
    ierr = EPSDenseSchur(ncv,eps->nconv,T,ncv,U,eps->eigr,eps->eigi);CHKERRQ(ierr);

    ierr = PetscMemzero(Ul,ncv*ncv*sizeof(PetscScalar));CHKERRQ(ierr);
    for (i=0;i<ncv;i++) { Ul[i*(ncv+1)] = 1.0; }
    ierr = EPSDenseSchur(ncv,eps->nconv,Tl,ncv,Ul,eigr,eigi);CHKERRQ(ierr);

    /* Sort the remaining columns of the Schur form */
    ierr = EPSSortDenseSchur(eps,ncv,eps->nconv,T,PETSC_NULL,ncv,U,PETSC_NULL,eps->eigr,eps->eigi);CHKERRQ(ierr);
    ierr = EPSSortDenseSchur(eps,ncv,eps->nconv,Tl,PETSC_NULL,ncv,Ul,PETSC_NULL,eigr,eigi);CHKERRQ(ierr);

    /* Compute residual norm estimates */
    ierr = ArnoldiResiduals(T,ncv,U,betav,eps->nconv,ncv,eps->eigr,eps->eigi,eps->errest,work);CHKERRQ(ierr);
    ierr = ArnoldiResiduals(Tl,ncv,Ul,betaw,eps->nconv,ncv,eigr,eigi,eps->errest_left,work);CHKERRQ(ierr);

    /* Lock converged eigenpairs and update the corresponding vectors,
       including the restart vector: V(:,idx) = V*U(:,idx); W(:,idx) = W*Ul(:,idx) */
    k = eps->nconv;
    while (k<ncv && eps->errest[k]<eps->tol && eps->errest_left[k]<eps->tol && PetscAbsScalar(eps->eigr[k]-eigr[k])<eps->tol) k++;
    //while (k<ncv && eps->errest[k]<eps->tol && eps->errest_left[k]<eps->tol) k++;
    ierr = SlepcUpdateVectors(ncv,eps->V,eps->nconv,PetscMin(k+1,ncv),U,ncv,PETSC_FALSE);CHKERRQ(ierr);
    ierr = SlepcUpdateVectors(ncv,eps->W,eps->nconv,PetscMin(k+1,ncv),Ul,ncv,PETSC_FALSE);CHKERRQ(ierr);

    eps->nconv = k;
    EPSMonitor(eps,eps->its,eps->nconv,eps->eigr,eps->eigi,eps->errest,ncv);
    EPSMonitor(eps,eps->its,eps->nconv,eigr,eigi,eps->errest_left,ncv);
    if (eps->nconv >= eps->nev) break;

    /* Clean tridiagonal matrices */
    for (i=k;i<ncv;i++) for (j=k+2;j<ncv;j++) { T[i+j*ncv]=0.0; Tl[i+j*ncv]=0.0; }
  }
  
  if( eps->nconv >= eps->nev ) eps->reason = EPS_CONVERGED_TOL;
  else eps->reason = EPS_DIVERGED_ITS;

  ierr = PetscFree(ritz);CHKERRQ(ierr);
  ierr = PetscFree(bnd);CHKERRQ(ierr);
  //ierr = PetscFree(Y);CHKERRQ(ierr);
  ierr = PetscFree(U);CHKERRQ(ierr);
  ierr = PetscFree(Ul);CHKERRQ(ierr);
  ierr = PetscFree(E);CHKERRQ(ierr);
  ierr = PetscFree(work);CHKERRQ(ierr);
  ierr = PetscFree(perm);CHKERRQ(ierr);
  ierr = PetscFree(delta);CHKERRQ(ierr);
  ierr = PetscFree(eigr);CHKERRQ(ierr);
  ierr = PetscFree(eigi);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}


