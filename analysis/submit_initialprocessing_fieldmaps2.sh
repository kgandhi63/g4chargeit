#!/bin/bash
#SBATCH --job-name=process-SWcase-adjustedworld
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:00:00
#SBATCH --output=outputlogs/log-PEonly

source ~/.bashrc
conda activate geant4-env

echo "processing results for dissipationRefinedGrid-initial8max0.8final11 (PE case, 40 um sphere)"
python process-fieldmaps.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.8final11-nodissipation-adjustedWorld" 'PE_425K_initial8max0.8final11_noDissipation_sphere40um_pos-0.1_through1XX' 'onlyphotoemission' 40
date
