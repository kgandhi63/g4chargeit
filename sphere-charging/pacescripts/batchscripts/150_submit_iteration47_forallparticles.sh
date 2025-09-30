#!/bin/bash
#SBATCH --job-name=Iteration47_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration47_allparticles_%A

module load openmpi/4.1.5

echo "Starting iteration47 for allparticles configuration"
srun ./charging_sphere macros/147_stackediteration47_allparticles_from_00_num10000.mac
date
