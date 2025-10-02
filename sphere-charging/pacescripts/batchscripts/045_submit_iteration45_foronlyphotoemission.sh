#!/bin/bash
#SBATCH --job-name=Iteration45_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=32gb
#SBATCH --time=20:00:00
#SBATCH --output=outputlogs/iteration45_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration45 for onlyphotoemission configuration"
srun ./charging_sphere macros/045_stackediteration45_onlyphotoemission_num500000.mac
date
