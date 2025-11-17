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
python process-fieldmaps.py "../build-zimmerman-comparison" 'PE_425K_initial8max0.8final12_RefinedGridDissipation_sphere40um_adjustedworld_through37' 'onlyphotoemission' 40
date
