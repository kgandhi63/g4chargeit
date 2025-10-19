#!/bin/bash
#SBATCH --job-name=Iteration9_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration9_onlyphotoemission_%A

echo "Starting iteration9 for onlyphotoemission configuration"
srun ./charging_sphere macros/110_stackediteration9_onlyphotoemission_from_00_num10000.mac
date
