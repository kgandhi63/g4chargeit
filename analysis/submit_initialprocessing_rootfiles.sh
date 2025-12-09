#!/bin/bash
#SBATCH --job-name=process-test
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=256gb
#SBATCH --time=02:00:00
#SBATCH --output=outputlogs/log-rootprocessing-PEcasezimmerman

echo "combine all root files up to a specific iteration number"
python process-rootfiles.py "../build-irregular-spheres" "irregularInitial8max0.8final11_PEparticleloc_until46" 'onlyphotoemission' 46
date