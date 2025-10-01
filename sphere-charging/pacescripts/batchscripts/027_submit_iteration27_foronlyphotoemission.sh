#!/bin/bash
#SBATCH --job-name=Iteration27_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=1gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration27_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration27 for onlyphotoemission configuration"
srun ./charging_sphere macros/027_stackediteration27_onlyphotoemission_num500000.mac
date
