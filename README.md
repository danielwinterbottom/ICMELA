# ICMELA
package to run MELA

## Setup the Workarea
setup CMSSW area
```
cd $CMSSW_BASE/src
cmsenv
git cms-addpkg FWCore/Version
```

clone the IC analysis framework
```
git clone git@github.com:ajgilbert/ICHiggsTauTau.git UserCode/ICHiggsTauTau
```
clone and setup the MELA package
```
git clone -b v2.1.1 git@github.com:cms-analysis/HiggsAnalysis-ZZMatrixElement ZZMatrixElement
cd ZZMatrixElement && ./setup.sh -j 8
cd $CMSSW_BASE/src
```

build
```
scram b -j8
```
