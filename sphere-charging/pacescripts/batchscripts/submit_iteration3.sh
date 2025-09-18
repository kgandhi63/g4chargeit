#!/bin/bash
#SBATCH --job-name=Iteration3_Array
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
#SBATCH --output=outputlogs/iteration3_%A_%a.out
#SBATCH --array=1-2

# Read in the array of commands
readarray -t commands < runscripts/run_iteration3.sh

# Run the command corresponding to this array task
cmd="${commands[$SLURM_ARRAY_TASK_ID-1]}"
echo "Running task $SLURM_ARRAY_TASK_ID: $cmd"
eval "$cmd"

date
