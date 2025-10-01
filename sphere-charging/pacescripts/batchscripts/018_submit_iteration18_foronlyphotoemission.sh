#!/bin/bash
#SBATCH --job-name=Iteration18_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=1gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration18_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration18 for onlyphotoemission configuration"
srun ./charging_sphere macros/018_stackediteration18_onlyphotoemission_num100000.mac
date
