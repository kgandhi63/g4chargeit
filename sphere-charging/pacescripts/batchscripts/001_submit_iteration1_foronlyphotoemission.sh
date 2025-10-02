#!/bin/bash
#SBATCH --job-name=Iteration1_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=32gb
#SBATCH --time=20:00:00
#SBATCH --output=outputlogs/iteration1_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration1 for onlyphotoemission configuration"
srun ./charging_sphere macros/001_stackediteration1_onlyphotoemission_num500000.mac
date
