#!/bin/bash
#SBATCH --job-name=process-PEcase-through136
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:00:00
#SBATCH --output=outputlogs/log-PEcase-through136-40um-24workers

echo "processing results for dissipationRefinedGrid-initial8max0.8final12 (PE case, 40 um sphere)"
python process-fieldmaps.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-dissipationRefinedGrid-initial8max0.8final12" 'PE_425K_initial8max0.8final12_RefinedGridDissipation_sphere40um_posx-0.1z0.35_through136' 'onlyphotoemission' 40
date
