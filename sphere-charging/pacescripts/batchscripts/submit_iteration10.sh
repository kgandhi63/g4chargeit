#!/bin/bash
#SBATCH --job-name=Iteration10_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
#SBATCH --output=outputlogs/iteration10_%A.out

echo "Starting iteration10 runs"
bash runscripts/run_iteration10.sh
date
