#!/bin/bash
#SBATCH --job-name=Iteration27_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration27_allparticles_%A

module load openmpi/4.1.5

echo "Starting iteration27 for allparticles configuration"
srun ./charging_sphere macros/127_stackediteration27_allparticles_from_00_num10000.mac
date
