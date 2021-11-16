/*
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   SLEPc - Scalable Library for Eigenvalue Problem Computations
   Copyright (c) 2002-2021, Universitat Politecnica de Valencia, Spain

   This file is part of SLEPc.
   SLEPc is distributed under a 2-clause BSD license (see LICENSE).
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
/*
   NEP routines related to monitors
*/

#include <slepc/private/nepimpl.h>      /*I "slepcnep.h" I*/
#include <petscdraw.h>

PetscErrorCode NEPMonitorLGCreate(MPI_Comm comm,const char host[],const char label[],const char metric[],PetscInt l,const char *names[],int x,int y,int m,int n,PetscDrawLG *lgctx)
{
  PetscDraw      draw;
  PetscDrawAxis  axis;
  PetscDrawLG    lg;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscDrawCreate(comm,host,label,x,y,m,n,&draw);CHKERRQ(ierr);
  ierr = PetscDrawSetFromOptions(draw);CHKERRQ(ierr);
  ierr = PetscDrawLGCreate(draw,l,&lg);CHKERRQ(ierr);
  if (names) { ierr = PetscDrawLGSetLegend(lg,names);CHKERRQ(ierr); }
  ierr = PetscDrawLGSetFromOptions(lg);CHKERRQ(ierr);
  ierr = PetscDrawLGGetAxis(lg,&axis);CHKERRQ(ierr);
  ierr = PetscDrawAxisSetLabels(axis,"Convergence","Iteration",metric);CHKERRQ(ierr);
  ierr = PetscDrawDestroy(&draw);CHKERRQ(ierr);
  *lgctx = lg;
  PetscFunctionReturn(0);
}

/*
   Runs the user provided monitor routines, if any.
*/
PetscErrorCode NEPMonitor(NEP nep,PetscInt it,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest)
{
  PetscErrorCode ierr;
  PetscInt       i,n = nep->numbermonitors;

  PetscFunctionBegin;
  for (i=0;i<n;i++) {
    ierr = (*nep->monitor[i])(nep,it,nconv,eigr,eigi,errest,nest,nep->monitorcontext[i]);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorSet - Sets an ADDITIONAL function to be called at every
   iteration to monitor the error estimates for each requested eigenpair.

   Logically Collective on nep

   Input Parameters:
+  nep     - eigensolver context obtained from NEPCreate()
.  monitor - pointer to function (if this is NULL, it turns off monitoring)
.  mctx    - [optional] context for private data for the
             monitor routine (use NULL if no context is desired)
-  monitordestroy - [optional] routine that frees monitor context (may be NULL)

   Calling Sequence of monitor:
$   monitor(NEP nep,int its,int nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal* errest,int nest,void *mctx)

+  nep    - nonlinear eigensolver context obtained from NEPCreate()
.  its    - iteration number
.  nconv  - number of converged eigenpairs
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates for each eigenpair
.  nest   - number of error estimates
-  mctx   - optional monitoring context, as set by NEPMonitorSet()

   Options Database Keys:
+    -nep_monitor        - print only the first error estimate
.    -nep_monitor_all    - print error estimates at each iteration
.    -nep_monitor_conv   - print the eigenvalue approximations only when
      convergence has been reached
.    -nep_monitor draw::draw_lg - sets line graph monitor for the first unconverged
      approximate eigenvalue
.    -nep_monitor_all draw::draw_lg - sets line graph monitor for all unconverged
      approximate eigenvalues
.    -nep_monitor_conv draw::draw_lg - sets line graph monitor for convergence history
-    -nep_monitor_cancel - cancels all monitors that have been hardwired into
      a code by calls to NEPMonitorSet(), but does not cancel those set via
      the options database.

   Notes:
   Several different monitoring routines may be set by calling
   NEPMonitorSet() multiple times; all will be called in the
   order in which they were set.

   Level: intermediate

.seealso: NEPMonitorFirst(), NEPMonitorAll(), NEPMonitorCancel()
@*/
PetscErrorCode NEPMonitorSet(NEP nep,PetscErrorCode (*monitor)(NEP,PetscInt,PetscInt,PetscScalar*,PetscScalar*,PetscReal*,PetscInt,void*),void *mctx,PetscErrorCode (*monitordestroy)(void**))
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  if (nep->numbermonitors >= MAXNEPMONITORS) SETERRQ(PetscObjectComm((PetscObject)nep),PETSC_ERR_ARG_OUTOFRANGE,"Too many NEP monitors set");
  nep->monitor[nep->numbermonitors]           = monitor;
  nep->monitorcontext[nep->numbermonitors]    = (void*)mctx;
  nep->monitordestroy[nep->numbermonitors++]  = monitordestroy;
  PetscFunctionReturn(0);
}

/*@
   NEPMonitorCancel - Clears all monitors for a NEP object.

   Logically Collective on nep

   Input Parameters:
.  nep - eigensolver context obtained from NEPCreate()

   Options Database Key:
.    -nep_monitor_cancel - Cancels all monitors that have been hardwired
      into a code by calls to NEPMonitorSet(),
      but does not cancel those set via the options database.

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorCancel(NEP nep)
{
  PetscErrorCode ierr;
  PetscInt       i;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  for (i=0; i<nep->numbermonitors; i++) {
    if (nep->monitordestroy[i]) {
      ierr = (*nep->monitordestroy[i])(&nep->monitorcontext[i]);CHKERRQ(ierr);
    }
  }
  nep->numbermonitors = 0;
  PetscFunctionReturn(0);
}

/*@C
   NEPGetMonitorContext - Gets the monitor context, as set by
   NEPMonitorSet() for the FIRST monitor only.

   Not Collective

   Input Parameter:
.  nep - eigensolver context obtained from NEPCreate()

   Output Parameter:
.  ctx - monitor context

   Level: intermediate

.seealso: NEPMonitorSet(), NEPDefaultMonitor()
@*/
PetscErrorCode NEPGetMonitorContext(NEP nep,void *ctx)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  *(void**)ctx = nep->monitorcontext[0];
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorFirst - Print the first unconverged approximate value and
   error estimate at each iteration of the nonlinear eigensolver.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor - activates NEPMonitorFirst()

   Level: intermediate

.seealso: NEPMonitorSet(), NEPMonitorAll(), NEPMonitorConverged()
@*/
PetscErrorCode NEPMonitorFirst(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode ierr;
  PetscViewer    viewer = vf->viewer;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  if (its==1 && ((PetscObject)nep)->prefix) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Eigenvalue approximations and residual norms for %s solve.\n",((PetscObject)nep)->prefix);CHKERRQ(ierr);
  }
  if (nconv<nest) {
    ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
    ierr = PetscViewerASCIIAddTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
    ierr = PetscViewerASCIIPrintf(viewer,"%3" PetscInt_FMT " NEP nconv=%" PetscInt_FMT " first unconverged value (error)",its,nconv);CHKERRQ(ierr);
    ierr = PetscViewerASCIIUseTabs(viewer,PETSC_FALSE);CHKERRQ(ierr);
#if defined(PETSC_USE_COMPLEX)
    ierr = PetscViewerASCIIPrintf(viewer," %g%+gi",(double)PetscRealPart(eigr[nconv]),(double)PetscImaginaryPart(eigr[nconv]));CHKERRQ(ierr);
#else
    ierr = PetscViewerASCIIPrintf(viewer," %g",(double)eigr[nconv]);CHKERRQ(ierr);
    if (eigi[nconv]!=0.0) { ierr = PetscViewerASCIIPrintf(viewer,"%+gi",(double)eigi[nconv]);CHKERRQ(ierr); }
#endif
    ierr = PetscViewerASCIIPrintf(viewer," (%10.8e)\n",(double)errest[nconv]);CHKERRQ(ierr);
    ierr = PetscViewerASCIIUseTabs(viewer,PETSC_TRUE);CHKERRQ(ierr);
    ierr = PetscViewerASCIISubtractTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
    ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorAll - Print the current approximate values and
   error estimates at each iteration of the nonlinear eigensolver.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor_all - activates NEPMonitorAll()

   Level: intermediate

.seealso: NEPMonitorSet(), NEPMonitorFirst(), NEPMonitorConverged()
@*/
PetscErrorCode NEPMonitorAll(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode ierr;
  PetscInt       i;
  PetscViewer    viewer = vf->viewer;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
  ierr = PetscViewerASCIIAddTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
  if (its==1 && ((PetscObject)nep)->prefix) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Eigenvalue approximations and residual norms for %s solve.\n",((PetscObject)nep)->prefix);CHKERRQ(ierr);
  }
  ierr = PetscViewerASCIIPrintf(viewer,"%3" PetscInt_FMT " NEP nconv=%" PetscInt_FMT " Values (Errors)",its,nconv);CHKERRQ(ierr);
  ierr = PetscViewerASCIIUseTabs(viewer,PETSC_FALSE);CHKERRQ(ierr);
  for (i=0;i<nest;i++) {
#if defined(PETSC_USE_COMPLEX)
    ierr = PetscViewerASCIIPrintf(viewer," %g%+gi",(double)PetscRealPart(eigr[i]),(double)PetscImaginaryPart(eigr[i]));CHKERRQ(ierr);
#else
    ierr = PetscViewerASCIIPrintf(viewer," %g",(double)eigr[i]);CHKERRQ(ierr);
    if (eigi[i]!=0.0) { ierr = PetscViewerASCIIPrintf(viewer,"%+gi",(double)eigi[i]);CHKERRQ(ierr); }
#endif
    ierr = PetscViewerASCIIPrintf(viewer," (%10.8e)",(double)errest[i]);CHKERRQ(ierr);
  }
  ierr = PetscViewerASCIIPrintf(viewer,"\n");CHKERRQ(ierr);
  ierr = PetscViewerASCIIUseTabs(viewer,PETSC_TRUE);CHKERRQ(ierr);
  ierr = PetscViewerASCIISubtractTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
  ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorConverged - Print the approximate values and
   error estimates as they converge.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor_conv - activates NEPMonitorConverged()

   Level: intermediate

.seealso: NEPMonitorSet(), NEPMonitorFirst(), NEPMonitorAll()
@*/
PetscErrorCode NEPMonitorConverged(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode ierr;
  PetscInt       i;
  PetscViewer    viewer = vf->viewer;
  SlepcConvMon   ctx;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  ctx = (SlepcConvMon)vf->data;
  if (its==1 && ((PetscObject)nep)->prefix) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Convergence history for %s solve.\n",((PetscObject)nep)->prefix);CHKERRQ(ierr);
  }
  if (its==1) ctx->oldnconv = 0;
  if (ctx->oldnconv!=nconv) {
    ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
    ierr = PetscViewerASCIIAddTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
    for (i=ctx->oldnconv;i<nconv;i++) {
      ierr = PetscViewerASCIIPrintf(viewer,"%3" PetscInt_FMT " NEP converged value (error) #%" PetscInt_FMT,its,i);CHKERRQ(ierr);
      ierr = PetscViewerASCIIUseTabs(viewer,PETSC_FALSE);CHKERRQ(ierr);
#if defined(PETSC_USE_COMPLEX)
      ierr = PetscViewerASCIIPrintf(viewer," %g%+gi",(double)PetscRealPart(eigr[i]),(double)PetscImaginaryPart(eigr[i]));CHKERRQ(ierr);
#else
      ierr = PetscViewerASCIIPrintf(viewer," %g",(double)eigr[i]);CHKERRQ(ierr);
      if (eigi[i]!=0.0) { ierr = PetscViewerASCIIPrintf(viewer,"%+gi",(double)eigi[i]);CHKERRQ(ierr); }
#endif
      ierr = PetscViewerASCIIPrintf(viewer," (%10.8e)\n",(double)errest[i]);CHKERRQ(ierr);
      ierr = PetscViewerASCIIUseTabs(viewer,PETSC_TRUE);CHKERRQ(ierr);
    }
    ierr = PetscViewerASCIISubtractTab(viewer,((PetscObject)nep)->tablevel);CHKERRQ(ierr);
    ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
    ctx->oldnconv = nconv;
  }
  PetscFunctionReturn(0);
}

PetscErrorCode NEPMonitorConvergedCreate(PetscViewer viewer,PetscViewerFormat format,void *ctx,PetscViewerAndFormat **vf)
{
  PetscErrorCode ierr;
  SlepcConvMon   mctx;

  PetscFunctionBegin;
  ierr = PetscViewerAndFormatCreate(viewer,format,vf);CHKERRQ(ierr);
  ierr = PetscNew(&mctx);CHKERRQ(ierr);
  mctx->ctx = ctx;
  (*vf)->data = (void*)mctx;
  PetscFunctionReturn(0);
}

PetscErrorCode NEPMonitorConvergedDestroy(PetscViewerAndFormat **vf)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (!*vf) PetscFunctionReturn(0);
  ierr = PetscFree((*vf)->data);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&(*vf)->viewer);CHKERRQ(ierr);
  ierr = PetscDrawLGDestroy(&(*vf)->lg);CHKERRQ(ierr);
  ierr = PetscFree(*vf);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorFirstDrawLG - Plots the error estimate of the first unconverged
   approximation at each iteration of the nonlinear eigensolver.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor draw::draw_lg - activates NEPMonitorFirstDrawLG()

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorFirstDrawLG(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode ierr;
  PetscViewer    viewer = vf->viewer;
  PetscDrawLG    lg = vf->lg;
  PetscReal      x,y;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  PetscValidHeaderSpecific(lg,PETSC_DRAWLG_CLASSID,8);
  ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
  if (its==1) {
    ierr = PetscDrawLGReset(lg);CHKERRQ(ierr);
    ierr = PetscDrawLGSetDimension(lg,1);CHKERRQ(ierr);
    ierr = PetscDrawLGSetLimits(lg,1,1,PetscLog10Real(nep->tol)-2,0.0);CHKERRQ(ierr);
  }
  if (nconv<nest) {
    x = (PetscReal)its;
    if (errest[nconv] > 0.0) y = PetscLog10Real(errest[nconv]);
    else y = 0.0;
    ierr = PetscDrawLGAddPoint(lg,&x,&y);CHKERRQ(ierr);
    if (its <= 20 || !(its % 5) || nep->reason) {
      ierr = PetscDrawLGDraw(lg);CHKERRQ(ierr);
      ierr = PetscDrawLGSave(lg);CHKERRQ(ierr);
    }
  }
  ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorFirstDrawLGCreate - Creates the plotter for the first error estimate.

   Collective on viewer

   Input Parameters:
+  viewer - the viewer
.  format - the viewer format
-  ctx    - an optional user context

   Output Parameter:
.  vf     - the viewer and format context

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorFirstDrawLGCreate(PetscViewer viewer,PetscViewerFormat format,void *ctx,PetscViewerAndFormat **vf)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscViewerAndFormatCreate(viewer,format,vf);CHKERRQ(ierr);
  (*vf)->data = ctx;
  ierr = NEPMonitorLGCreate(PetscObjectComm((PetscObject)viewer),NULL,"First Error Estimate","Log Error Estimate",1,NULL,PETSC_DECIDE,PETSC_DECIDE,400,300,&(*vf)->lg);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorAllDrawLG - Plots the error estimate of all unconverged
   approximations at each iteration of the nonlinear eigensolver.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor_all draw::draw_lg - activates NEPMonitorAllDrawLG()

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorAllDrawLG(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode ierr;
  PetscViewer    viewer = vf->viewer;
  PetscDrawLG    lg = vf->lg;
  PetscInt       i,n = PetscMin(nep->nev,255);
  PetscReal      *x,*y;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  PetscValidHeaderSpecific(lg,PETSC_DRAWLG_CLASSID,8);
  ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
  if (its==1) {
    ierr = PetscDrawLGReset(lg);CHKERRQ(ierr);
    ierr = PetscDrawLGSetDimension(lg,n);CHKERRQ(ierr);
    ierr = PetscDrawLGSetLimits(lg,1,1,PetscLog10Real(nep->tol)-2,0.0);CHKERRQ(ierr);
  }
  ierr = PetscMalloc2(n,&x,n,&y);CHKERRQ(ierr);
  for (i=0;i<n;i++) {
    x[i] = (PetscReal)its;
    if (i < nest && errest[i] > 0.0) y[i] = PetscLog10Real(errest[i]);
    else y[i] = 0.0;
  }
  ierr = PetscDrawLGAddPoint(lg,x,y);CHKERRQ(ierr);
  if (its <= 20 || !(its % 5) || nep->reason) {
    ierr = PetscDrawLGDraw(lg);CHKERRQ(ierr);
    ierr = PetscDrawLGSave(lg);CHKERRQ(ierr);
  }
  ierr = PetscFree2(x,y);CHKERRQ(ierr);
  ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorAllDrawLGCreate - Creates the plotter for all the error estimates.

   Collective on viewer

   Input Parameters:
+  viewer - the viewer
.  format - the viewer format
-  ctx    - an optional user context

   Output Parameter:
.  vf     - the viewer and format context

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorAllDrawLGCreate(PetscViewer viewer,PetscViewerFormat format,void *ctx,PetscViewerAndFormat **vf)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscViewerAndFormatCreate(viewer,format,vf);CHKERRQ(ierr);
  (*vf)->data = ctx;
  ierr = NEPMonitorLGCreate(PetscObjectComm((PetscObject)viewer),NULL,"All Error Estimates","Log Error Estimate",1,NULL,PETSC_DECIDE,PETSC_DECIDE,400,300,&(*vf)->lg);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorConvergedDrawLG - Plots the number of converged eigenvalues
   at each iteration of the nonlinear eigensolver.

   Collective on nep

   Input Parameters:
+  nep    - nonlinear eigensolver context
.  its    - iteration number
.  its    - iteration number
.  nconv  - number of converged eigenpairs so far
.  eigr   - real part of the eigenvalues
.  eigi   - imaginary part of the eigenvalues
.  errest - error estimates
.  nest   - number of error estimates to display
-  vf     - viewer and format for monitoring

   Options Database Key:
.  -nep_monitor_conv draw::draw_lg - activates NEPMonitorConvergedDrawLG()

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorConvergedDrawLG(NEP nep,PetscInt its,PetscInt nconv,PetscScalar *eigr,PetscScalar *eigi,PetscReal *errest,PetscInt nest,PetscViewerAndFormat *vf)
{
  PetscErrorCode   ierr;
  PetscViewer      viewer = vf->viewer;
  PetscDrawLG      lg = vf->lg;
  PetscReal        x,y;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(nep,NEP_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,8);
  PetscValidHeaderSpecific(lg,PETSC_DRAWLG_CLASSID,8);
  ierr = PetscViewerPushFormat(viewer,vf->format);CHKERRQ(ierr);
  if (its==1) {
    ierr = PetscDrawLGReset(lg);CHKERRQ(ierr);
    ierr = PetscDrawLGSetDimension(lg,1);CHKERRQ(ierr);
    ierr = PetscDrawLGSetLimits(lg,1,1,0,nep->nev);CHKERRQ(ierr);
  }
  x = (PetscReal)its;
  y = (PetscReal)nep->nconv;
  ierr = PetscDrawLGAddPoint(lg,&x,&y);CHKERRQ(ierr);
  if (its <= 20 || !(its % 5) || nep->reason) {
    ierr = PetscDrawLGDraw(lg);CHKERRQ(ierr);
    ierr = PetscDrawLGSave(lg);CHKERRQ(ierr);
  }
  ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
   NEPMonitorConvergedDrawLGCreate - Creates the plotter for the convergence history.

   Collective on viewer

   Input Parameters:
+  viewer - the viewer
.  format - the viewer format
-  ctx    - an optional user context

   Output Parameter:
.  vf     - the viewer and format context

   Level: intermediate

.seealso: NEPMonitorSet()
@*/
PetscErrorCode NEPMonitorConvergedDrawLGCreate(PetscViewer viewer,PetscViewerFormat format,void *ctx,PetscViewerAndFormat **vf)
{
  PetscErrorCode ierr;
  SlepcConvMon   mctx;

  PetscFunctionBegin;
  ierr = PetscViewerAndFormatCreate(viewer,format,vf);CHKERRQ(ierr);
  ierr = PetscNew(&mctx);CHKERRQ(ierr);
  mctx->ctx = ctx;
  (*vf)->data = (void*)mctx;
  ierr = NEPMonitorLGCreate(PetscObjectComm((PetscObject)viewer),NULL,"Convergence History","Number Converged",1,NULL,PETSC_DECIDE,PETSC_DECIDE,400,300,&(*vf)->lg);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

