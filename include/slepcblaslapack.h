/*

  Necessary routines in BLAS and LAPACK not included in petscblaslapack.f


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

#if !defined(__SLEPCBLASLAPACK_H)
#define __SLEPCBLASLAPACK_H
#include "petscblaslapack.h"

/* Macros for building LAPACK names */
#if defined(PETSC_BLASLAPACK_UNDERSCORE)
#if defined(PETSC_USE_REAL_SINGLE)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) s##lcase##_
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) c##lcase##_
#else
#define SLEPC_BLASLAPACK(lcase,ucase) s##lcase##_
#endif
#elif defined(PETSC_USE_REAL___FLOAT128)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) q##lcase##_
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) w##lcase##_
#else
#define SLEPC_BLASLAPACK(lcase,ucase) q##lcase##_
#endif
#else
#define SLEPC_BLASLAPACKREAL(lcase,ucase) d##lcase##_
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) z##lcase##_
#else
#define SLEPC_BLASLAPACK(lcase,ucase) d##lcase##_
#endif
#endif

#elif defined(PETSC_BLASLAPACK_CAPS) || defined(PETSC_BLASLAPACK_STDCALL)
#if defined(PETSC_USE_REAL_SINGLE)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) S##ucase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) C##ucase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) S##ucase
#endif
#elif defined(PETSC_USE_REAL___FLOAT128)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) Q##ucase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) W##ucase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) Q##ucase
#endif
#else
#define SLEPC_BLASLAPACKREAL(lcase,ucase) D##ucase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) Z##ucase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) D##ucase
#endif
#endif

#else
#if defined(PETSC_USE_REAL_SINGLE)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) s##lcase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) c##lcase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) s##lcase
#endif
#elif defined(PETSC_USE_REAL___FLOAT128)
#define SLEPC_BLASLAPACKREAL(lcase,ucase) q##lcase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) w##lcase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) q##lcase
#endif
#else
#define SLEPC_BLASLAPACKREAL(lcase,ucase) d##lcase
#if defined(PETSC_USE_COMPLEX)
#define SLEPC_BLASLAPACK(lcase,ucase) z##lcase
#else
#define SLEPC_BLASLAPACK(lcase,ucase) d##lcase
#endif
#endif

#endif

/* LAPACK functions without string parameters */
#define LAPACKlaev2_ SLEPC_BLASLAPACK(laev2,LAEV2)
#define LAPACKgehrd_ SLEPC_BLASLAPACK(gehrd,GEHRD)
#define LAPACKgetri_ SLEPC_BLASLAPACK(getri,GETRI)
#define LAPACKgelqf_ SLEPC_BLASLAPACK(gelqf,GELQF)
#define LAPACKtgexc_ SLEPC_BLASLAPACK(tgexc,TGEXC)
#define LAPACKlag2_  SLEPC_BLASLAPACKREAL(lag2,LAG2)
#define LAPACKlasv2_ SLEPC_BLASLAPACKREAL(lasv2,LASV2)
#if !defined(PETSC_USE_COMPLEX)
#define LAPACKorghr_ SLEPC_BLASLAPACK(orghr,ORGHR)
#define LAPACKorgqr_ SLEPC_BLASLAPACK(orgqr,ORGQR)
#else
#define LAPACKorghr_ SLEPC_BLASLAPACK(unghr,UNGHR)
#define LAPACKorgqr_ SLEPC_BLASLAPACK(ungqr,UNGQR)
#endif
/* the next one needs a special treatment due to the special names:
   srot, drot, csrot, zdrot */
#if !defined(PETSC_USE_COMPLEX)
#define BLASrot_     SLEPC_BLASLAPACK(rot,ROT)
#else
#if defined(PETSC_USE_REAL_SINGLE)
#define BLASrot_     SLEPC_BLASLAPACK(srot,SROT)
#elif defined(PETSC_USE_REAL___FLOAT128)
#define BLASrot_     SLEPC_BLASLAPACK(qrot,QROT)
#else
#define BLASrot_     SLEPC_BLASLAPACK(drot,DROT)
#endif
#endif

/* LAPACK functions with string parameters */
#if !defined(PETSC_BLASLAPACK_STDCALL)

/* same name for real and complex */
#define BLAStrsm_(a,b,c,d,e,f,g,h,i,j,k) SLEPC_BLASLAPACK(trsm,TRSM) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),1,1,1,1)
#define LAPACKlanhs_(a,b,c,d,e) SLEPC_BLASLAPACK(lanhs,LANHS) ((a),(b),(c),(d),(e),1)
#define LAPACKlange_(a,b,c,d,e,f) SLEPC_BLASLAPACK(lange,LANGE) ((a),(b),(c),(d),(e),(f),1)
#define LAPACKggevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac) SLEPC_BLASLAPACK(ggevx,GGEVX) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w),(x),(y),(z),(aa),(ab),(ac),1,1,1,1)
#define LAPACKggev_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) SLEPC_BLASLAPACK(ggev,GGEV) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),1,1)
#define LAPACKpbtrf_(a,b,c,d,e,f) SLEPC_BLASLAPACK(pbtrf,PBTRF) ((a),(b),(c),(d),(e),(f),1)
#define LAPACKsteqr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(steqr,STEQR) ((a),(b),(c),(d),(e),(f),(g),(h),1)
/* subroutines in which we use only the real version, do not care whether they have different name */
#define LAPACKstevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) SLEPC_BLASLAPACKREAL(stevr,STEVR) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),1,1)
#define LAPACKbdsdc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACKREAL(bdsdc,BDSDC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),1,1)
#define LAPACKlamch_(a) SLEPC_BLASLAPACKREAL(lamch,LAMCH) ((a),1)

#if !defined(PETSC_USE_COMPLEX)
/* different name or signature, real */
#define BLASsymm_(a,b,c,d,e,f,g,h,i,j,k,l) SLEPC_BLASLAPACK(symm,SYMM) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),1,1)
#define LAPACKsyevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) SLEPC_BLASLAPACK(syevr,SYEVR) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),1,1,1)
#define LAPACKsygvd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n)  SLEPC_BLASLAPACK(sygvd,SYGVD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),1,1)
#define LAPACKormlq_(a,b,c,d,e,f,g,h,i,j,k,l,m) SLEPC_BLASLAPACK(ormlq,ORMLQ) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),1,1)
#define LAPACKorgtr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(orgtr,ORGTR) ((a),(b),(c),(d),(e),(f),(g),(h),1)
#define LAPACKsytrd_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(sytrd,SYTRD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),1)
#define LAPACKtrevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACK(trevc,TREVC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),1,1)
#define LAPACKgeevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) SLEPC_BLASLAPACK(geevx,GEEVX) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w),1,1,1,1)
#define LAPACKtrexc_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(trexc,TREXC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),1)
#define LAPACKgesdd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACK(gesdd,GESDD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),1)
#define LAPACKtgevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) SLEPC_BLASLAPACK(tgevc,TGEVC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),1,1)
#else
/* different name or signature, complex */
#define BLASsymm_(a,b,c,d,e,f,g,h,i,j,k,l) SLEPC_BLASLAPACK(hemm,HEMM) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),1,1)
#define LAPACKsyevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) SLEPC_BLASLAPACK(heevr,HEEVR) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w),1,1,1)
#define LAPACKsygvd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) SLEPC_BLASLAPACK(hegvd,HEGVD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),1,1)
#define LAPACKormlq_(a,b,c,d,e,f,g,h,i,j,k,l,m) SLEPC_BLASLAPACK(unmlq,UNMLQ) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),1,1)
#define LAPACKorgtr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(ungtr,UNGTR) ((a),(b),(c),(d),(e),(f),(g),(h),1)
#define LAPACKsytrd_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(hetrd,HETRD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),1)
#define LAPACKtrevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) SLEPC_BLASLAPACK(trevc,TREVC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),1,1)
#define LAPACKgeevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) SLEPC_BLASLAPACK(geevx,GEEVX) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),1,1,1,1)
#define LAPACKtrexc_(a,b,c,d,e,f,g,h,i) SLEPC_BLASLAPACK(trexc,TREXC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),1)
#define LAPACKgesdd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) SLEPC_BLASLAPACK(gesdd,GESDD) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),1)
#define LAPACKtgevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) SLEPC_BLASLAPACK(tgevc,TGEVC) ((a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),1,1)
#endif

#else /* PETSC_BLASLAPACK_STDCALL */

/* same name for real and complex */
#define BLAStrsm_(a,b,c,d,e,f,g,h,i,j,k) SLEPC_BLASLAPACK(trsm,TRSM) ((a),1,(b),1,(c),1,(d),1,(e),(f),(g),(h),(i),(j),(k))
#define LAPACKlanhs_(a,b,c,d,e) SLEPC_BLASLAPACK(lanhs,LANHS) ((a),1,(b),(c),(d),(e))
#define LAPACKlange_(a,b,c,d,e,f) SLEPC_BLASLAPACK(lange,LANGE) ((a),1,(b),(c),(d),(e),(f))
#define LAPACKggevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac) SLEPC_BLASLAPACK(ggevx,GGEVX) ((a),1,(b),1,(c),1,(d),1,(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w),(x),(y),(z),(aa),(ab),(ac))
#define LAPACKggev_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) SLEPC_BLASLAPACK(ggev,GGEV) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q))
#define LAPACKpbtrf_(a,b,c,d,e,f) SLEPC_BLASLAPACK(pbtrf,PBTRF) ((a),1,(b),(c),(d),(e),(f))
#define LAPACKsteqr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(steqr,STEQR) ((a),1,(b),(c),(d),(e),(f),(g),(h))
/* subroutines in which we use only the real version, do not care whether they have different name */
#define LAPACKstevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) SLEPC_BLASLAPACKREAL(stevr,STEVR) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t))
#define LAPACKbdsdc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACKREAL(bdsdc,BDSDC) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n))
#define LAPACKlamch_(a) SLEPC_BLASLAPACKREAL(lamch,LAMCH) ((a),1)

#if !defined(PETSC_USE_COMPLEX)
/* different name or signature, real */
#define BLASsymm_(a,b,c,d,e,f,g,h,i,j,k,l) SLEPC_BLASLAPACK(symm,SYMM) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l))
#define LAPACKsyevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) SLEPC_BLASLAPACK(syevr,SYEVR) ((a),1,(b),1,(c),1,(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u))
#define LAPACKsygvd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n)  SLEPC_BLASLAPACK(sygvd,SYGVD) ((a),(b),1,(c),1,(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n))
#define LAPACKormlq_(a,b,c,d,e,f,g,h,i,j,k,l,m) SLEPC_BLASLAPACK(ormlq,ORMLQ) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m))
#define LAPACKorgtr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(orgtr,ORGTR) ((a),1,(b),(c),(d),(e),(f),(g),(h))
#define LAPACKsytrd_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(sytrd,SYTRD) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i),(j))
#define LAPACKtrevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACK(trevc,TREVC) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n))
#define LAPACKgeevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) SLEPC_BLASLAPACK(geevx,GEEVX) ((a),1,(b),1,(c),1,(d),1,(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w))
#define LAPACKtrexc_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(trexc,TREXC) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i),(j))
#define LAPACKgesdd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n) SLEPC_BLASLAPACK(gesdd,GESDD) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n))
#define LAPACKtgevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) SLEPC_BLASLAPACK(tgevc,TGEVC) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p))
#else
/* different name or signature, complex */
#define BLASsymm_(a,b,c,d,e,f,g,h,i,j,k,l) SLEPC_BLASLAPACK(hemm,HEMM) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l))
#define LAPACKsyevr_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) SLEPC_BLASLAPACK(heevr,HEEVR) ((a),1,(b),1,(c),1,(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v),(w))
#define LAPACKsygvd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) SLEPC_BLASLAPACK(hegvd,HEGVD) ((a),(b),1,(c),1,(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p))
#define LAPACKormlq_(a,b,c,d,e,f,g,h,i,j,k,l,m) SLEPC_BLASLAPACK(unmlq,UNMLQ) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m))
#define LAPACKorgtr_(a,b,c,d,e,f,g,h) SLEPC_BLASLAPACK(ungtr,UNGTR) ((a),1,(b),(c),(d),(e),(f),(g),(h))
#define LAPACKsytrd_(a,b,c,d,e,f,g,h,i,j) SLEPC_BLASLAPACK(hetrd,HETRD) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i),(j))
#define LAPACKtrevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) SLEPC_BLASLAPACK(trevc,TREVC) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o))
#define LAPACKgeevx_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) SLEPC_BLASLAPACK(geevx,GEEVX) ((a),1,(b),1,(c),1,(d),1,(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q),(r),(s),(t),(u),(v))
#define LAPACKtrexc_(a,b,c,d,e,f,g,h,i) SLEPC_BLASLAPACK(trexc,TREXC) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i))
#define LAPACKgesdd_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) SLEPC_BLASLAPACK(gesdd,GESDD) ((a),1,(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o))
#define LAPACKtgevc_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) SLEPC_BLASLAPACK(tgevc,TGEVC) ((a),1,(b),1,(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),(p),(q))
#endif

#endif

PETSC_EXTERN_CXX_BEGIN
EXTERN_C_BEGIN

#if !defined(PETSC_BLASLAPACK_STDCALL)

/* LAPACK functions without string parameters */
extern void      SLEPC_BLASLAPACK(laev2,LAEV2) (PetscScalar*,PetscScalar*,PetscScalar*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*);
extern void      SLEPC_BLASLAPACK(gehrd,GEHRD) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(getri,GETRI) (PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(gelqf,GELQF) (PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACKREAL(lag2,LAG2) (PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*);
extern void      SLEPC_BLASLAPACKREAL(lasv2,LASV2) (PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*);
extern void      BLASrot_(PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*);
#if !defined(PETSC_USE_COMPLEX)
extern void      SLEPC_BLASLAPACK(tgexc,TGEXC) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(orghr,ORGHR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(orgqr,ORGQR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
#else
extern void      SLEPC_BLASLAPACK(tgexc,TGEXC) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(unghr,UNGHR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void      SLEPC_BLASLAPACK(ungqr,UNGQR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
#endif

/* LAPACK functions with string parameters */
extern void      SLEPC_BLASLAPACK(trsm,TRSM)   (const char*,const char*,const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern PetscReal SLEPC_BLASLAPACK(lanhs,LANHS) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt);
extern PetscReal SLEPC_BLASLAPACK(lange,LANGE) (const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt);
extern PetscReal SLEPC_BLASLAPACK(pbtrf,PBTRF) (const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(steqr,STEQR) (const char*,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt);

extern void      SLEPC_BLASLAPACKREAL(stevr,STEVR) (const char*,const char*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACKREAL(bdsdc,BDSDC) (const char*,const char*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern PetscReal SLEPC_BLASLAPACKREAL(lamch,LAMCH) (const char*,PetscBLASInt);

#if !defined(PETSC_USE_COMPLEX)
extern void      SLEPC_BLASLAPACK(ggevx,GGEVX) (const char*,const char*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(ggev,GGEV) (const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(symm,SYMM)   (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*, PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(syevr,SYEVR) (const char*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt);        
extern void      SLEPC_BLASLAPACK(sygvd,SYGVD) (PetscBLASInt*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(ormlq,ORMLQ) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(orgtr,ORGTR) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(sytrd,SYTRD) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(trevc,TREVC) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(geevx,GEEVX) (const char*,const char*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(trexc,TREXC) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(gesdd,GESDD) (const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(tgevc,TGEVC) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
#else
extern void      SLEPC_BLASLAPACK(ggevx,GGEVX) (const char*,const char*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*, PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*, PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(ggev,GGEV) (const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(hemm,HEMM)   (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*, PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(heevr,HEEVR) (const char *,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscBLASInt*, PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(hegvd,HEGVD) (PetscBLASInt*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(unmlq,UNMLQ) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(ungtr,UNGTR) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(hetrd,HETRD) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(trevc,TREVC) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscReal*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(geevx,GEEVX) (const char*,const char*,const char*,const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt,PetscBLASInt,PetscBLASInt,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(trexc,TREXC) (const char*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(gesdd,GESDD) (const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt);
extern void      SLEPC_BLASLAPACK(tgevc,TGEVC) (const char*,const char*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscReal*,PetscBLASInt*,PetscBLASInt,PetscBLASInt);
#endif

#else /* PETSC_BLASLAPACK_STDCALL */

/* LAPACK functions without string parameters */
extern void PETSC_STDCALL SLEPC_BLASLAPACK(laev2,LAEV2) (PetscScalar*,PetscScalar*,PetscScalar*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(gehrd,GEHRD) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(getri,GETRI) (PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(gelqf,GELQF) (PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACKREAL(lag2,LAG2) (PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*);
extern void PETSC_STDCALL SLEPC_BLASLAPACKREAL(lasv2,LASV2) (PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*);
extern void PETSC_STDCALL BLASrot_(PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*);
#if !defined(PETSC_USE_COMPLEX)
extern void PETSC_STDCALL SLEPC_BLASLAPACK(tgexc,TGEXC) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(orghr,ORGHR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(orgqr,ORGQR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
#else
extern void PETSC_STDCALL SLEPC_BLASLAPACK(tgexc,TGEXC) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(unghr,UNGHR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ungqr,UNGQR) (PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
#endif

/* LAPACK functions with string parameters */
extern void PETSC_STDCALL SLEPC_BLASLAPACK(trsm,TRSM)   (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*);
extern PetscReal PETSC_STDCALL SLEPC_BLASLAPACK(lanhs,LANHS) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*);
extern PetscReal PETSC_STDCALL SLEPC_BLASLAPACK(lange,LANGE) (const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*);
extern PetscReal PETSC_STDCALL SLEPC_BLASLAPACK(pbtrf,PBTRF) (const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(steqr,STEQR) (const char*,PetscBLASInt,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*);

extern void PETSC_STDCALL SLEPC_BLASLAPACKREAL(stevr,STEVR) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACKREAL(bdsdc,BDSDC) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*);
extern PetscReal SLEPC_BLASLAPACKREAL(lamch,LAMCH) (const char*,PetscBLASInt);

#if !defined(PETSC_USE_COMPLEX)
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ggevx,GGEVX) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ggev,GGEV) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(symm,SYMM)   (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*, PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(syevr,SYEVR) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(sygvd,SYGVD) (PetscBLASInt*,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ormlq,ORMLQ) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(orgtr,ORGTR) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(sytrd,SYTRD) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(trevc,TREVC) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(geevx,GEEVX) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(trexc,TREXC) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(gesdd,GESDD) (const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(tgevc,TGEVC) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*);
#else
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ggevx,GGEVX) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*, PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*, PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ggev,GGEV) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(hemm,HEMM)   (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscScalar*, PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(heevr,HEEVR) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscBLASInt*, PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(hegvd,HEGVD) (PetscBLASInt*,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(unmlq,UNMLQ) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(ungtr,UNGTR) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(hetrd,HETRD) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscReal*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(trevc,TREVC) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscReal*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(geevx,GEEVX) (const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(trexc,TREXC) (const char*,PetscBLASInt,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(gesdd,GESDD) (const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscReal*,PetscBLASInt*,PetscBLASInt*);
extern void PETSC_STDCALL SLEPC_BLASLAPACK(tgevc,TGEVC) (const char*,PetscBLASInt,const char*,PetscBLASInt,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscScalar*,PetscBLASInt*,PetscBLASInt*,PetscBLASInt*,PetscScalar*,PetscReal*,PetscBLASInt*);
#endif

#endif

EXTERN_C_END
PETSC_EXTERN_CXX_END

#endif
