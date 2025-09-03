#!/bin/bash
#SBATCH --job-name=Iteration0_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
#SBATCH --output=outputlogs/iteration0_%A.out

echo "Starting iteration0 runs"
bash runscripts/run_iteration0.sh
date
