#!/bin/bash
#SBATCH --job-name=Iteration102_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration102_onlysolarwind_%A

echo "Starting iteration102 for onlysolarwind configuration"
srun ./charging_sphere macros/102_stackediteration102_onlysolarwind_from_00_num5000.mac
date
