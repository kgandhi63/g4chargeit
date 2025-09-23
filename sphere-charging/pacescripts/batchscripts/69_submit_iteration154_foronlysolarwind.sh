#!/bin/bash
#SBATCH --job-name=Iteration154_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration154_onlysolarwind_%A

echo "Starting iteration154 for onlysolarwind configuration"
srun ./charging_sphere macros/154_stackediteration154_onlysolarwind_from_00_num5000.mac
date
