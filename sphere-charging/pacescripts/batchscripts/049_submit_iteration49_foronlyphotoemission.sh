#!/bin/bash
#SBATCH --job-name=Iteration49_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem-per-cpu=14gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/iteration49_onlyphotoemission_%A

echo "Starting iteration49 for onlyphotoemission configuration"
srun ./charging_sphere macros/049_stackediteration49_onlyphotoemission_num100.mac
date
