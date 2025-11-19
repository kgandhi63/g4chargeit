#!/bin/bash
#SBATCH --job-name=process-test
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/log-testrootprocessing

source ~/.bashrc
conda activate geant4-env

echo "processing results for dissipationRefinedGrid-initial8max0.8final11 (PE case, 40 um sphere)"
python process-rootfiles.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.8final11-adjustedWorld-allcases" "PE_425K_initial8max0.8final11_particleloc" 'onlyphotoemission' 131
python process-rootfiles.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.8final11-adjustedWorld-allcases" "SW_425K_initial8max0.8final11_particleloc" 'onlysolarwind' 57
date