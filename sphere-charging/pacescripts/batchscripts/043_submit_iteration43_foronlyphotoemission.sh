#!/bin/bash
#SBATCH --job-name=Iteration43_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem-per-cpu=14gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/iteration43_onlyphotoemission_%A

echo "Starting iteration43 for onlyphotoemission configuration"
srun ./charging_sphere macros/043_stackediteration43_onlyphotoemission_num100.mac
date
