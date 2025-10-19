#!/bin/bash
#SBATCH --job-name=Iteration90_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration90_allparticles_%A

echo "Starting iteration90 for allparticles configuration"
srun ./charging_sphere macros/292_stackediteration90_allparticles_from_00_num10000.mac
date
