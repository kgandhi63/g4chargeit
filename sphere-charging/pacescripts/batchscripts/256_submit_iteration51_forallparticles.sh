#!/bin/bash
#SBATCH --job-name=Iteration51_Configurationallparticles
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration51_allparticles_%A

echo "Starting iteration51 for allparticles configuration"
srun ./charging_sphere macros/253_stackediteration51_allparticles_from_00_num5000.mac
date
