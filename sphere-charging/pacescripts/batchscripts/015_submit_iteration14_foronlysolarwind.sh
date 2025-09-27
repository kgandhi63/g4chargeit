#!/bin/bash
#SBATCH --job-name=Iteration14_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration14_onlysolarwind_%A

echo "Starting iteration14 for onlysolarwind configuration"
srun ./charging_sphere macros/014_stackediteration14_onlysolarwind_from_00_num2000.mac
date
