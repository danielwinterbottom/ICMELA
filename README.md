# ICMELA
package to run MELA

## Setup the Workarea
setup CMSSW area
```
cd $CMSSW_BASE/src
cmsenv
git cms-addpkg FWCore/Version
```
clone and setup the MELA package
```
git clone -b v2.1.1 git@github.com:cms-analysis/HiggsAnalysis-ZZMatrixElement ZZMatrixElement
cd ZZMatrixElement ; ./setup.sh ; cd ..
```

build
```
scram b -j8
```

## MELA Documentation
MELA manual: http://hroskes.web.cern.ch/hroskes/JHUGen/manJHUGenerator.pdf


## Submitting the jobs
! grid submission currently not working see below for infomation on batch submission

Move the SVFit input file directory over to ICMELA/MELATEST/<local-folder>, then copy the input files over to dcache (e.g. ICMELA/MELATEST/scripts/copy_svfit_input_files_to_dcache.py or something better). Note that for this code to work the input files on dcache need to all be in the same directory. The files in ICMELA/MELATEST/<local-folder> need to be the same as those in path/to/input/folder/on/dcache

To submit the jobs (after sourcing the crab environment):
```
cd ICMELA/MELATEST
./scripts/crabsub.py -i <local-folder> --name <request name> --area <crab area name> --file_prefix /path/to/input/folder/on/dcache
```
The file_prefix needs to have the full path to the file i.e 'root://gfe02.grid.hep.ph.ic.ac.uk:1097' before the '/store/user/...'

Can then check on the status of jobs with the standard crab commands, once the jobs are done copy the output files back over to the analysis area and untar (crab getoutput should work, then recursive untar) 

submit as batch jobs:
``
cd ICMELA/MELATEST
./scripts/batch_sub.py -i /vols/cms/dw515/Offline/output/SM/Jan22_MELA/ --submit --jobsub='./scripts/submit_ic_batch_job.sh "hep.q -l h_rt=0:180:0"' --parajobs
``

or to submit batch jobs for multiple systematic files use:
``
cd ICMELA/MELATEST
dirs=('' 'TSCALE_UP' 'TSCALE_DOWN' 'TSCALE0PI_UP' 'TSCALE0PI_DOWN' 'TSCALE1PI_UP' 'TSCALE1PI_DOWN' 'TSCALE3PRONG_UP' 'TSCALE3PRONG_DOWN' 'EFAKE0PI_UP' 'EFAKE0PI_DOWN' 'EFAKE1PI_UP' 'EFAKE1PI_DOWN' 'MUFAKE0PI_UP' 'MUFAKE0PI_DOWN' 'MUFAKE1PI_UP' 'MUFAKE1PI_DOWN' 'METUNCL_UP' 'METUNCL_DOWN' 'METCL_UP' 'METCL_DOWN' 'MUSCALE_UP' 'MUSCALE_DOWN' 'ESCALE_UP' 'ESCALE_DOWN' 'JESCENT_DOWN' 'JESCENT_UP' 'JESFULL_DOWN' 'JESFULL_UP' 'JESHF_DOWN' 'JESHF_UP' 'JESRBAL_DOWN' 'JESRBAL_UP')

for i in "${dirs[@]}"; do ./scripts/batch_sub.py -i /vols/cms/dw515/Offline/output/SM/Jan22_MELA/$i/ --submit --jobsub='./scripts/submit_ic_batch_job.sh "hep.q -l h_rt=0:180:0"' --parajobs; done

``
## Alternative instructions which tend to be quicker
!grid submissions currently not working!
copy mela inputs to batch using:
```
./scripts/copy_to_dcache.sh path/to/mela/inputs path/to/dcache/dir 1 job_name 
```
or if running also for the systematic shifted inputs do:

```
dirs=('' 'TSCALE_UP' 'TSCALE_DOWN' 'TSCALE0PI_UP' 'TSCALE0PI_DOWN' 'TSCALE1PI_UP' 'TSCALE1PI_DOWN' 'TSCALE3PRONG_UP' 'TSCALE3PRONG_DOWN' 'EFAKE0PI_UP' 'EFAKE0PI_DOWN' 'EFAKE1PI_UP' 'EFAKE1PI_DOWN' 'MUFAKE0PI_UP' 'MUFAKE0PI_DOWN' 'MUFAKE1PI_UP' 'MUFAKE1PI_DOWN' 'METUNCL_UP' 'METUNCL_DOWN' 'METCL_UP' 'METCL_DOWN' 'MUSCALE_UP' 'MUSCALE_DOWN' 'ESCALE_UP' 'ESCALE_DOWN' 'JESCENT_DOWN' 'JESCENT_UP' 'JESFULL_DOWN' 'JESFULL_UP' 'JESHF_DOWN' 'JESHF_UP' 'JESRBAL_DOWN' 'JESRBAL_UP')

for i in "${dirs[@]}"; do ./scripts/copy_to_dcache.sh /path/to/mela/inputs/$i path/to/dcache/dir/$i 1 $i; done
```
then submit the jobs using:
```
python scripts/submit_crab_jobs.py --folder=/vols/cms/dw515/Offline/output/SM/Jan22_MELA/ --dcache_dir=/store/user/dwinterb/Jan22_MELA/ --copy
```
the --copy option will check that the mela input files exist on the dcache directory and then copy them over if they don't so is only needed to make sure all the inputs were copied correctly in the first step

Once the job have finished the .tar files can be copied using the command:

```
python scripts/copy_crab_outputs.py --folder=/path/to/target/directory/ --dcache_dir=/path/to/dcache/output/directory/
```
which assumes the target directroy has the corret subdirectory structure for the systematic shifted folders. This script also check if the .tar files exists before copying it over so exisiting files won't be overwritten.

The following script can be use to check that the number of .tar output fils matched the number of SV fit input files if they are stored in the same folder:
```
./scripts/check_copied.sh /vols/cms/dw515/Offline/output/SM/Dec29_SVFit/
```
This script will also unntar the output files. If the numbers don't match you shoud re-run the copying script.
