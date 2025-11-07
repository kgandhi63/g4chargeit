#!/bin/bash
#SBATCH --job-name=process-initial8max0.8final12
#SBATCH --account=gts-zjiang33
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=264gb
#SBATCH --time=01:00:00
#SBATCH --output=outputlogs/log-425K-withTotalParticleDissipation-50umsphere

echo "processing results for initial8max0.8final12 (temp 425K with dissipation)"
python process-fieldmaps.py "../build-dissipationTotalParticles-initial8max0.8final12" 'PE_425K_initial8max0.8final12_withTotalParticleDissipation_sphere50um' 'onlyphotoemission' 50
date
