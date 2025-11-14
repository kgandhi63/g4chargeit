#!/bin/bash
#SBATCH --job-name=process-SWcase-through5X
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:00:00
#SBATCH --output=outputlogs/log-SWcase-through5X

echo "processing results for dissipationRefinedGrid-initial8max0.8final12 (SW case, 20 um sphere)"
python process-fieldmaps.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-dissipationRefinedGrid-initial8max0.8final12" 'SW_425K_initial8max0.8final12_RefinedGridDissipation_sphere20um_pos-0.1_through5X' 'onlysolarwind' 20
date
