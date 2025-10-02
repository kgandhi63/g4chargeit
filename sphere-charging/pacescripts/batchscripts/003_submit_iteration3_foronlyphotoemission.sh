#!/bin/bash
#SBATCH --job-name=Iteration3_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=32gb
#SBATCH --time=20:00:00
#SBATCH --output=outputlogs/iteration3_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration3 for onlyphotoemission configuration"
srun ./charging_sphere macros/003_stackediteration3_onlyphotoemission_num500000.mac
date
