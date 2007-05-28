
/*
      Spectral transformation module for eigenvalue problems.  
*/
#if !defined(__SLEPCST_H)
#define __SLEPCST_H
#include "petscksp.h"
PETSC_EXTERN_CXX_BEGIN

extern PetscCookie ST_COOKIE;

typedef struct _p_ST* ST;

#define STSHELL     "shell"
#define STSHIFT     "shift"
#define STSINV      "sinvert"
#define STCAYLEY    "cayley"
#define STFOLD      "fold"
#define STType const char*

EXTERN PetscErrorCode STCreate(MPI_Comm,ST*);
EXTERN PetscErrorCode STDestroy(ST);
EXTERN PetscErrorCode STSetType(ST,STType);
EXTERN PetscErrorCode STGetType(ST,STType*);
EXTERN PetscErrorCode STSetOperators(ST,Mat,Mat);
EXTERN PetscErrorCode STGetOperators(ST,Mat*,Mat*);
EXTERN PetscErrorCode STSetUp(ST);
EXTERN PetscErrorCode STSetFromOptions(ST);
EXTERN PetscErrorCode STView(ST,PetscViewer);

EXTERN PetscErrorCode STApply(ST,Vec,Vec);
EXTERN PetscErrorCode STGetBilinearForm(ST,Mat*);
EXTERN PetscErrorCode STApplyTranspose(ST,Vec,Vec);
EXTERN PetscErrorCode STComputeExplicitOperator(ST,Mat*);
EXTERN PetscErrorCode STPostSolve(ST);

EXTERN PetscErrorCode STInitializePackage(char*);

EXTERN PetscErrorCode STSetKSP(ST,KSP);
EXTERN PetscErrorCode STGetKSP(ST,KSP*);
EXTERN PetscErrorCode STAssociatedKSPSolve(ST,Vec,Vec);
EXTERN PetscErrorCode STSetShift(ST,PetscScalar);
EXTERN PetscErrorCode STGetShift(ST,PetscScalar*);

EXTERN PetscErrorCode STSetOptionsPrefix(ST,const char*);
EXTERN PetscErrorCode STAppendOptionsPrefix(ST,const char*);
EXTERN PetscErrorCode STGetOptionsPrefix(ST,const char*[]);

EXTERN PetscErrorCode STBackTransform(ST,PetscScalar*,PetscScalar*);

EXTERN PetscErrorCode STCheckNullSpace(ST,int,const Vec[]);

EXTERN PetscErrorCode STGetOperationCounters(ST,int*,int*);
EXTERN PetscErrorCode STResetOperationCounters(ST);

typedef enum { STMATMODE_COPY, STMATMODE_INPLACE, 
               STMATMODE_SHELL } STMatMode;
EXTERN PetscErrorCode STSetMatMode(ST,STMatMode);
EXTERN PetscErrorCode STGetMatMode(ST,STMatMode*);
EXTERN PetscErrorCode STSetMatStructure(ST,MatStructure);

/* --------- options specific to particular spectral transformations-------- */

EXTERN PetscErrorCode STShellGetContext(ST st,void **ctx);
EXTERN PetscErrorCode STShellSetContext(ST st,void *ctx);
EXTERN PetscErrorCode STShellSetApply(ST st,PetscErrorCode (*apply)(void*,Vec,Vec));
EXTERN PetscErrorCode STShellSetApplyTranspose(ST st,PetscErrorCode (*applytrans)(void*,Vec,Vec));
EXTERN PetscErrorCode STShellSetBackTransform(ST st,PetscErrorCode (*backtr)(void*,PetscScalar*,PetscScalar*));
EXTERN PetscErrorCode STShellSetName(ST,const char[]);
EXTERN PetscErrorCode STShellGetName(ST,char*[]);

EXTERN PetscErrorCode STCayleySetAntishift(ST,PetscScalar);

EXTERN PetscErrorCode STFoldSetLeftSide(ST st,PetscTruth left);

PETSC_EXTERN_CXX_END
#endif

