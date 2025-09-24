#!/bin/bash
#SBATCH --job-name=Iteration15_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration15_onlyphotoemission_%A

echo "Starting iteration15 for onlyphotoemission configuration"
srun ./charging_sphere macros/116_stackediteration15_onlyphotoemission_from_00_num3000.mac
date
