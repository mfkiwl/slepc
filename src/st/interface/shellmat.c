/*
      This file contains the subroutines which implement various operations
      of the matrix associated to the shift-and-invert technique for eigenvalue
      problems, and also a subroutine to create it.
*/

#include "src/st/stimpl.h"

#undef __FUNCT__
#define __FUNCT__ "STMatShellMult"
static int STMatShellMult(Mat A,Vec x,Vec y)
{
  int         ierr;
  ST          ctx;
  PetscScalar alpha;

  PetscFunctionBegin;
  ierr = MatShellGetContext(A,(void**)&ctx);CHKERRQ(ierr);

  ierr = MatMult(ctx->A,x,y);CHKERRQ(ierr);
  if (ctx->sigma != 0.0) { 
    alpha = -ctx->sigma;
    if (ctx->B) {  /* y = (A - sB) x */
      ierr = MatMult(ctx->B,x,ctx->w);CHKERRQ(ierr);
      ierr = VecAXPY(&alpha,ctx->w,y);CHKERRQ(ierr); 
    }
    else {    /* y = (A - sI) x */
    ierr = VecAXPY(&alpha,x,y);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "STMatShellGetDiagonal"
static int STMatShellGetDiagonal(Mat A,Vec diag)
{
  int         ierr;
  ST          ctx;
  PetscScalar alpha;
  Vec         diagb;

  PetscFunctionBegin;
  ierr = MatShellGetContext(A,(void**)&ctx);CHKERRQ(ierr);

  ierr = MatGetDiagonal(ctx->A,diag);CHKERRQ(ierr);
  if (ctx->sigma != 0.0) { 
    alpha = -ctx->sigma;
    if (ctx->B) {
      ierr = VecDuplicate(diag,&diagb);CHKERRQ(ierr);
      ierr = MatGetDiagonal(ctx->B,diagb);CHKERRQ(ierr);
      ierr = VecAXPY(&alpha,diagb,diag);CHKERRQ(ierr);
      ierr = VecDestroy(diagb);CHKERRQ(ierr);
    }
    else {
      ierr = VecShift(&alpha,diag);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "STMatShellCreate"
int STMatShellCreate(ST st,Mat *mat)
{
  int          n, m, N, M, ierr;
  PetscTruth   hasA, hasB;

  PetscFunctionBegin;
  ierr = MatGetSize(st->A,&M,&N);CHKERRQ(ierr);  
  ierr = MatGetLocalSize(st->A,&m,&n);CHKERRQ(ierr);  
  ierr = MatCreateShell(st->comm,m,n,M,N,(void*)st,mat);CHKERRQ(ierr);
  ierr = MatShellSetOperation(*mat,MATOP_MULT,(void(*)(void))STMatShellMult);CHKERRQ(ierr);

  ierr = MatHasOperation(st->A,MATOP_GET_DIAGONAL,&hasA);CHKERRQ(ierr);
  if (st->B) { ierr = MatHasOperation(st->B,MATOP_GET_DIAGONAL,&hasB);CHKERRQ(ierr); }
  if ( (hasA && !st->B) || (hasA && hasB) ) {
    ierr = MatShellSetOperation(*mat,MATOP_GET_DIAGONAL,(void(*)(void))STMatShellGetDiagonal);CHKERRQ(ierr);
  }

  PetscFunctionReturn(0);
}

