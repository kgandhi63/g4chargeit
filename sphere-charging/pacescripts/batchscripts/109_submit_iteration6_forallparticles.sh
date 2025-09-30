#!/bin/bash
#SBATCH --job-name=Iteration6_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration6_allparticles_%A

module load openmpi/4.1.5

echo "Starting iteration6 for allparticles configuration"
srun ./charging_sphere macros/106_stackediteration6_allparticles_from_00_num10000.mac
date
