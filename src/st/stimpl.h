
#ifndef _STIMPL
#define _STIMPL

#include "slepceps.h"

extern PetscEvent ST_SetUp, ST_Apply, ST_ApplyB, ST_ApplyTranspose;
extern PetscFList STList;

typedef struct _STOps *STOps;

struct _STOps {
  int          (*setup)(ST);
  int          (*apply)(ST,Vec,Vec);
  int          (*getbilinearform)(ST,Mat*);
  int          (*applytrans)(ST,Vec,Vec);
  int          (*setshift)(ST,PetscScalar);
  int          (*setfromoptions)(ST);
  int          (*postsolve)(ST);  
  int          (*backtr)(ST,PetscScalar*,PetscScalar*);  
  int          (*destroy)(ST);
  int          (*view)(ST,PetscViewer);
};

struct _p_ST {
  PETSCHEADER(struct _STOps);
  /*------------------------- User parameters --------------------------*/
  Mat            A,B;              /* Matrices which define the eigensystem */
  PetscScalar    sigma;            /* Value of the shift */
  STMatMode      shift_matrix;
  MatStructure   str;          /* whether matrices have the same pattern or not */
  Mat            mat;

  /*------------------------- Misc data --------------------------*/
  KSP          ksp;
  Vec          w;
  void         *data;
  int          setupcalled;
  int          lineariterations;
  int          applys;
  int          (*checknullspace)(ST,int,const Vec[]);
  
};

EXTERN PetscErrorCode STRegisterAll(char*);
EXTERN PetscErrorCode STRegister(const char*,const char*,const char*,int(*)(ST));
#if defined(PETSC_USE_DYNAMIC_LIBRARIES)
#define STRegisterDynamic(a,b,c,d) STRegister(a,b,c,0)
#else
#define STRegisterDynamic(a,b,c,d) STRegister(a,b,c,d)
#endif

EXTERN PetscErrorCode STGetBilinearForm_Default(ST,Mat*);
EXTERN PetscErrorCode STView_Default(ST,PetscViewer);
EXTERN PetscErrorCode STAssociatedKSPSolve(ST,Vec,Vec);
EXTERN PetscErrorCode STAssociatedKSPSolveTranspose(ST,Vec,Vec);
EXTERN PetscErrorCode STCheckNullSpace_Default(ST,int,const Vec[]);
EXTERN PetscErrorCode STMatShellCreate(ST st,Mat *mat);

#endif

