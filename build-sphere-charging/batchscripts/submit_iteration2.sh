#!/bin/bash
#SBATCH --job-name=Iteration2_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=10:00:00
#SBATCH --output=outputlogs/iteration2.out

echo "Starting iteration2 runs"
bash runscripts/run_iteration2.sh
date
