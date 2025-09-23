#!/bin/bash
#SBATCH --job-name=Iteration174_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration174_onlyphotoemission_%A

echo "Starting iteration174 for onlyphotoemission configuration"
srun ./charging_sphere macros/374_stackediteration174_onlyphotoemission_from_00_num5000.mac
date
