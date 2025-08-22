#!/bin/bash
#SBATCH --job-name=Iteration1_Array
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=10:00:00
#SBATCH --output=outputlogs/iteration1_%A_%a.out
#SBATCH --array=1-5

# Read in the array of commands
readarray -t commands < runscripts/run_iteration1.sh

# Run the command corresponding to this array task
cmd="${commands[$SLURM_ARRAY_TASK_ID-1]}"
echo "Running task $SLURM_ARRAY_TASK_ID: $cmd"
eval "$cmd"

date
