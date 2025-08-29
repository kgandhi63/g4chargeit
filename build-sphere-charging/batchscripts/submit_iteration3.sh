#!/bin/bash
#SBATCH --job-name=Iteration3_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --mem=64gb
#SBATCH --time=10:00:00
#SBATCH --output=outputlogs/iteration3.out

echo "Starting iteration3 runs"
bash runscripts/run_iteration3.sh
date
