#!/bin/bash
#SBATCH --job-name=Iteration1_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
#SBATCH --output=outputlogs/iteration1_onlyphotoemission_%A.out

echo "Starting iteration1 runs"
bash runscripts/run_iteration1_onlyphotoemission.sh
date
