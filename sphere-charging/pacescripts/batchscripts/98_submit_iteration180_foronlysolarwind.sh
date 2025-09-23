#!/bin/bash
#SBATCH --job-name=Iteration180_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration180_onlysolarwind_%A

echo "Starting iteration180 for onlysolarwind configuration"
srun ./charging_sphere macros/180_stackediteration180_onlysolarwind_from_00_num5000.mac
date
