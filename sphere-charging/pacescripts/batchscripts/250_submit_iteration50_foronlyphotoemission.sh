#!/bin/bash
#SBATCH --job-name=Iteration50_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration50_onlyphotoemission_%A

echo "Starting iteration50 for onlyphotoemission configuration"
srun ./charging_sphere macros/250_stackediteration50_onlyphotoemission_from_00_num5000.mac
date
