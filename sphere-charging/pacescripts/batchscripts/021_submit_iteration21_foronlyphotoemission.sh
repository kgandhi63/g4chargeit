#!/bin/bash
#SBATCH --job-name=Iteration21_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem-per-cpu=14gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/iteration21_onlyphotoemission_%A

echo "Starting iteration21 for onlyphotoemission configuration"
srun ./charging_sphere macros/021_stackediteration21_onlyphotoemission_num100.mac
date
