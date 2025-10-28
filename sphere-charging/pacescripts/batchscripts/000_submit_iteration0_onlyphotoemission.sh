#!/bin/bash
#SBATCH --job-name=Iteration0_Configurationonlyphotoemission
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --mem-per-cpu=10gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/%A_iteration0_onlyphotoemission

echo "Starting iteration0 for onlyphotoemission configuration"
./charging_sphere macros/000_iteration0_onlyphotoemission_num500000.mac
date
