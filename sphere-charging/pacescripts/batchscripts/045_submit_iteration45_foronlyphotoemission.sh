#!/bin/bash
#SBATCH --job-name=Iteration45_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem-per-cpu=14gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/iteration45_onlyphotoemission_%A

echo "Starting iteration45 for onlyphotoemission configuration"
srun ./charging_sphere macros/045_stackediteration45_onlyphotoemission_num100.mac
date
