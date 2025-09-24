#!/bin/bash
#SBATCH --job-name=Iteration62_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration62_onlyphotoemission_%A

echo "Starting iteration62 for onlyphotoemission configuration"
srun ./charging_sphere macros/163_stackediteration62_onlyphotoemission_from_00_num3000.mac
date
