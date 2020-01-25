#
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#  SLEPc - Scalable Library for Eigenvalue Problem Computations
#  Copyright (c) 2002-2020, Universitat Politecnica de Valencia, Spain
#
#  This file is part of SLEPc.
#  SLEPc is distributed under a 2-clause BSD license (see LICENSE).
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#

import os, shutil, log, package
try:
  from urllib import urlretrieve
except ImportError:
  from urllib.request import urlretrieve

class HPDDM(package.Package):

  def __init__(self,argdb,log):
    package.Package.__init__(self,argdb,log)
    self.packagename    = 'hpddm'
    self.downloadable   = True
    self.version        = 'd0c29caba379288f6780699e4bd58df6b7d44264'
    self.url            = 'https://github.com/hpddm/hpddm/archive/'+self.version+'.tar.gz'
    self.archive        = self.version+'.tar.gz'
    self.dirname        = 'hpddm-'+self.version
    self.supportssingle = True
    self.supports64bint = True
    self.ProcessArgs(argdb)

  def Precondition(self,petsc):
    pkg = self.packagename.upper()
    if petsc.cxxdialectcxx11 == False:
      self.log.Exit('ERROR: '+pkg+' requires C++11.')
    if petsc.buildsharedlib == False:
      self.log.Exit('ERROR: '+pkg+' requires a shared library build.')
    if petsc.hpddm == True:
      self.log.Exit('ERROR: '+pkg+' requires PETSc to be built without '+pkg+'.')
    package.Package.Precondition(self,petsc)

  def DownloadAndInstall(self,conf,vars,slepc,petsc,archdir):
    externdir = os.path.join(archdir,'externalpackages')
    builddir  = os.path.join(externdir,self.dirname)
    if slepc.prefixdir:
      installdir = slepc.prefixdir
    else:
      installdir = archdir
    self.Download(externdir,builddir)
    g = open(os.path.join(builddir,'SONAME_SL_LINKER'),'w')
    g.write('include '+os.path.join(petsc.dir,petsc.arch,'lib','petsc','conf','petscvariables')+'\n')
    g.write('soname:\n')
    g.write('\t@echo $(call SONAME_FUNCTION,'+os.path.join(installdir,'lib','libhpddm_petsc')+',0)\n')
    g.write('sl_linker:\n')
    g.write('\t@echo $(call SL_LINKER_FUNCTION,'+os.path.join(installdir,'lib','libhpddm_petsc')+',0,0)\n')
    g.close()
    d = petsc.dir+'/'+petsc.arch+'/lib'
    l = petsc.slflag+d+' -L'+d+' -lpetsc'
    d = installdir+'/lib'
    if petsc.arch is "":
      urlretrieve('https://www.mcs.anl.gov/petsc/petsc-master/src/ksp/ksp/impls/hpddm/hpddm.cxx',os.path.join(builddir,'interface','ksphpddm.cxx'));
      urlretrieve('https://www.mcs.anl.gov/petsc/petsc-master/src/ksp/pc/impls/hpddm/hpddm.cxx',os.path.join(builddir,'interface','pchpddm.cxx'));
    else:
      shutil.copyfile(os.path.join(petsc.dir,'src','ksp','ksp','impls','hpddm','hpddm.cxx'),os.path.join(builddir,'interface','ksphpddm.cxx'))
      shutil.copyfile(os.path.join(petsc.dir,'src','ksp','pc','impls','hpddm','hpddm.cxx'),os.path.join(builddir,'interface','pchpddm.cxx'))
    cmd = petsc.cxx+' '+petsc.cxx_flags+' -I'+os.path.join('.','include')+' -I'+os.path.join(petsc.dir,petsc.arch,'include')+' -I'+os.path.join(slepc.dir,'include')+' -I'+os.path.join(archdir,'include')+' -I'+os.path.join(petsc.dir,'include')+' -DPETSC_HAVE_SLEPC=1 -DSLEPC_LIB_DIR="'+d+'" -DPETSC_HAVE_SLEPC=1 '
    result,output = self.RunCommand('cd '+builddir+'&&'+cmd+' '+os.path.join('interface','ksphpddm.cxx')+' -c -o '+os.path.join('interface','ksphpddm.o')+'&&'+cmd+' '+os.path.join('interface','pchpddm.cxx')+' -c -o '+os.path.join('interface','pchpddm.o')+'&&'+cmd+' '+os.path.join('interface','hpddm_petsc.cpp')+' -c -o '+os.path.join('interface','hpddm_petsc.o'))
    if result:
      self.log.Exit('ERROR: compilation of HPDDM failed.')
    result,output = self.RunCommand('cd '+builddir+'&& make -f SONAME_SL_LINKER soname && make -f SONAME_SL_LINKER sl_linker')
    if result:
      self.log.Exit('ERROR: calling PETSc SONAME_FUNCTION or SL_LINKER_FUNCTION failed.')
    lines = output.splitlines()
    result,output = self.RunCommand('cd '+builddir+'&& '+petsc.cxx+' '+petsc.cxx_flags+' '+os.path.join('interface','hpddm_petsc.o')+' '+os.path.join('interface','pchpddm.o')+' '+os.path.join('interface','ksphpddm.o')+' -o '+lines[0]+' '+lines[1]+' '+l+' && ln -sf '+lines[0]+' '+os.path.join(d,'libhpddm_petsc.'+petsc.sl_suffix))
    if result:
      self.log.Exit('ERROR: installation of HPDDM failed.')
    for root, dirs, files in os.walk(os.path.join(builddir,'include')):
      for name in files:
        shutil.copyfile(os.path.join(builddir,'include',name),os.path.join(installdir,'include',name))
    l = petsc.slflag+d+' -L'+d+' -lhpddm_petsc'
    conf.write('#define SLEPC_HAVE_HPDDM 1\n')
    vars.write('HPDDM_LIB = '+l+'\n')
    vars.write('HPDDM_INCLUDE = -I'+os.path.join(installdir,'include')+'\n')
    self.packageflags = [l]
    self.havepackage = True

