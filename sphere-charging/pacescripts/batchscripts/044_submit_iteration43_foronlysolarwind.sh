#!/bin/bash
#SBATCH --job-name=Iteration43_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration43_onlysolarwind_%A

echo "Starting iteration43 for onlysolarwind configuration"
srun ./charging_sphere macros/043_stackediteration43_onlysolarwind_from_00_num500.mac
date
