#!/bin/bash
#SBATCH --job-name=Iteration0_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --mem-per-cpu=2048
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/%A_iteration0_onlysolarwind

echo "Starting iteration0 for onlysolarwind configuration"
./charging_sphere macros/000_iteration0_onlysolarwind_num500000.mac
date
