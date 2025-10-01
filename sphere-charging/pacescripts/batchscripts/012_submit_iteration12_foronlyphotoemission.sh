#!/bin/bash
#SBATCH --job-name=Iteration12_Configurationonlyphotoemission
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=1gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration12_onlyphotoemission_%A

module load openmpi/4.1.5

echo "Starting iteration12 for onlyphotoemission configuration"
srun ./charging_sphere macros/012_stackediteration12_onlyphotoemission_num500000.mac
date
