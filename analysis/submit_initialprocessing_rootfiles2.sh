#!/bin/bash
#SBATCH --job-name=process-test
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=256gb
#SBATCH --time=02:00:00
#SBATCH --output=outputlogs/log-rootprocessing-PEcasewang-series

echo "combine all root files up to a specific iteration number"
python process-rootfiles.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.7final11-wang" "WangInitial8max0.7final11_SWparticleloc_until3" 'onlysolarwind' 3
python process-rootfiles.py "/storage/scratch1/5/avira7/Grain-Charging-Simulation-Data/build-initial8max0.7final11-wang" "WangInitial8max0.7final11_SWparticleloc_until13" 'onlysolarwind' 13
date