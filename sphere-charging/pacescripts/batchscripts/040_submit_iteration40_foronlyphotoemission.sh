#!/bin/bash
#SBATCH --job-name=Iteration40_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=1gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration40_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration40 for onlyphotoemission configuration"
srun ./charging_sphere macros/040_stackediteration40_onlyphotoemission_num100000.mac
date
