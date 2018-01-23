#!/usr/bin/env python
import os
import fnmatch
from optparse import OptionParser

DRY_RUN = False
CRAB_PREFIX="""
set -x
set -e
ulimit -s unlimited
ulimit -c 0

function error_exit
{
  if [$1 -ne 0]; then
    echo "Error with exit code ${1}"
    if [ -e FrameworkJobReport.xml ]
    then
      cat << EOF > FrameworkJobReport.xml.tmp
      <FrameworkJobReport>
      <FrameworkError ExitStatus="${1}" Type="" >
      <Error with exit code ${1}
      </FrameworkError>
 EOF
     tail -n+2 FrameworkJobReport.xml >> FrameworkJobReport.xml.tmp
     mv FrameworkJobReport.xml.tmp FrameworkJobReport.xml
   else
     cat << EOF > FrameworkJobReport.xml
     <FrameworkJobReport>
     <FrameworkError ExitStatus="${1}" Type="" >
     Error with exit code ${1}
     </FrameworkError>
     </FrameworkJobReport>
EOF
     fi
     exit 0
   fi
 }

trap 'error_exit $?' ERR
"""
CRAB_POSTFIX="""
tar -cf mela_output.tar mela_*_output.root
rm mela_*_output.root
"""
parser = OptionParser()
parser.add_option("-i", dest= "input",
                    help="Input folder to scan")
parser.add_option("--name",dest="task_name",
                    help="Name of crab task")
parser.add_option("--area",dest="crab_area",
                    help="Crab area name")
parser.add_option("--file_prefix", dest="file_prefix",
                    help="Location of input files")


(options, args) = parser.parse_args()
task_name = options.task_name
crab_area = options.crab_area

from CRABAPI.RawCommand import crabCommand
from httplib import HTTPException
print '>> crab3 requestName will be %s' %task_name
outscriptname = 'crab_%s.sh' %task_name
print '>> crab3 script will be %s' %outscriptname
outscript = open(outscriptname, "w")
outscript.write(CRAB_PREFIX)
jobs=0
mela_files=set()
root = options.input
for filename in os.listdir(root) :
  if fnmatch.fnmatch(filename, 'mela_*_input.root'):
    jobs += 1
    fullfile = os.path.join(root, filename)
    mela_files.add(fullfile)
    outfile = fullfile.replace('input.root','output.root')
    outscript.write('\nif [ $1 -eq %i ]; then\n'%jobs)
    outscript.write("  ./MELATEST " +os.path.basename(fullfile) + " " + options.file_prefix + '\n')
    outscript.write('fi')
outscript.write(CRAB_POSTFIX)
outscript.close()
from ICMELA.MELATEST.crab import config
config.General.requestName = task_name
config.JobType.scriptExe = outscriptname
#config.JobType.inputFiles.extend(mela_files)
config.Data.totalUnits = jobs
config.Data.outputDatasetTag= config.General.requestName
if crab_area is not None:
  config.General.workArea = crab_area
if not DRY_RUN:
  try:
    crabCommand('submit', config=config)
  except HTTPException, hte:
    print hte.headers
