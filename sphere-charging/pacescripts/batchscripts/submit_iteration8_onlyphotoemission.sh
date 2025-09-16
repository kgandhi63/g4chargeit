#!/bin/bash
#SBATCH --job-name=Iteration8_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
#SBATCH --output=outputlogs/iteration8_onlyphotoemission_%A.out

echo "Starting iteration8 runs"
bash runscripts/run_iteration8_onlyphotoemission.sh
date
