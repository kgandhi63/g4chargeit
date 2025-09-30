#!/bin/bash
#SBATCH --job-name=Iteration42_Configurationonlysolarwind
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration42_onlysolarwind_%A

module load openmpi/4.1.5

echo "Starting iteration42 for onlysolarwind configuration"
srun ./charging_sphere macros/042_stackediteration42_onlysolarwind_from_00_num2000.mac
date
