#!/bin/bash
#SBATCH --job-name=Iteration136_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration136_allparticles_%A

echo "Starting iteration136 for allparticles configuration"
srun ./charging_sphere macros/536_stackediteration136_allparticles_from_00_num5000.mac
date
