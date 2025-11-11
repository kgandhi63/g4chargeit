#!/bin/bash
#SBATCH --job-name=process-initial8max0.8final12
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:00:00
#SBATCH --output=outputlogs/log-SW-dissipationRefinedGrid-initial8max0.8final12

echo "processing results for dissipationRefinedGrid-initial8max0.8final12 (SW case, 20 um sphere)"
python process-fieldmaps.py "../build-dissipationRefinedGrid-initial8max0.8final12" 'SW_425K_initial8max0.8final12_RefinedGridDissipation_sphere20um_pos-0.1_through4X' 'onlysolarwind' 20
date
