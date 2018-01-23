#python scripts/submit_crab_jobs.py --folder=/vols/cms/dw515/Offline/output/SM/Jan22_MELA/ --dcache_dir=/store/user/dwinterb/Jan22_MELA/ --copy

import sys
import os
import subprocess
from optparse import OptionParser
import math
import fnmatch

CRAB = 'Jan22_MELA'

parser = OptionParser()

parser.add_option("--folder", dest = "folder",
                  help="Specify local folder that contains the input files")
parser.add_option("--dcache_dir", dest = "dcache_dir",
                  help="Specify dcache directory that contains the input files")
parser.add_option("--copy", dest="copy", action='store_true', default=False,
                  help="Copy inputs to dcache")

(options,args) = parser.parse_args()

if not options.folder:
  parser.error('No folder specified')


subdirs = ['','TSCALE_DOWN','TSCALE_UP','TSCALE0PI_UP','TSCALE0PI_DOWN','TSCALE1PI_UP','TSCALE1PI_DOWN','TSCALE3PRONG_UP','TSCALE3PRONG_DOWN','EFAKE0PI_UP','EFAKE0PI_DOWN', 'EFAKE1PI_UP', 'EFAKE1PI_DOWN','MUFAKE0PI_UP','MUFAKE0PI_DOWN','MUFAKE1PI_UP','MUFAKE1PI_DOWN','METUNCL_UP','METUNCL_DOWN','METCL_UP','METCL_DOWN']
subdirs=['']


for subdir in subdirs:
  folder = '%s/%s/' %(options.folder,subdir)  
  dcache_dir = '/%s/%s/' % (options.dcache_dir,subdir)
  print 'Processing directory', folder 
  if options.copy:
    # first remove the files that we don't want to compute MELA for
    print 'Removing files that aren\'t needed..'
    if 'MUFAKE' in subdir or 'EFAKE' in subdir:
      os.system('ls %s/*.root | grep -v -e DY -e EWKZ | xargs rm' % (folder))
    if 'EFAKE' in subdir:
      os.system('ls %s/*.root | grep -v _et_ | xargs rm' % (folder))
    if 'MUFAKE' in subdir:
      os.system('ls %s/*.root | grep -v _mt_ | xargs rm' % (folder))
    if 'TSCALE_' in subdir:
      os.system('ls %s/*.root | grep -v _em_ | xargs rm' % (folder))
    if 'TSCALE' in subdir and 'TSCALE_' not in subdir:
      os.system('ls %s/*.root | grep _em_ | xargs rm' % (folder))
  
    # then copy the files over to the dcache
    print 'Copying files to dcache..'
    os.system('python scripts/copy_mela_input_files_to_dcache.py -i %s -d %s --checkExists' % (folder,dcache_dir)) 
  
  # check if all the inputs are on the dcache before submitting
  try: check_dcache = subprocess.check_output("xrd gfe02.grid.hep.ph.ic.ac.uk:1097 ls %s | grep input | grep .root" % dcache_dir, shell=True).split('\n')
  except: check_dcache = []
  try: check_dir = subprocess.check_output("ls %s/ | grep input.root" % folder, shell=True).split('\n')
  except: check_dir = []
  check_dir = [x for x in check_dir if '.root' in x and 'input' in x]
  check_dcache = [x for x in check_dcache if '.root' in x and 'input' in x]

  print  len(check_dcache), len(check_dir) 
  if len(check_dcache) != len(check_dir): 
    print subdir
    print "dcache and directory do not have the same number of input files, not submtting jobs"
    continue

  print 'Submitting jobs..'
  dcache_dir = 'root://gfe02.grid.hep.ph.ic.ac.uk:1097/%s/%s/' % (options.dcache_dir,subdir)
  name = '%s%s' % (CRAB,subdir)
  submit_command = './scripts/crabsub.py -i %s --name %s --area %s --file_prefix %s' % (folder,name,CRAB,dcache_dir)
  os.system(submit_command)
