#!/bin/bash
#SBATCH --job-name=process-allcases
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:30:00
#SBATCH --output=outputlogs/log-twocases-wang

source ~/.bashrc
conda activate geant4-env

echo "processing results for dissipationRefinedGrid-initial8max0.8final11 (PE case, 40 um sphere)"
python process-fieldmaps.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.6final10-wang" 'PEWang_425K_initial8max0.6final10_through111' 'onlyphotoemission' 10
python process-fieldmaps.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.7final11-wang" 'SWWang_425K_initial8max0.7final11_through120' 'onlysolarwind' 10
date