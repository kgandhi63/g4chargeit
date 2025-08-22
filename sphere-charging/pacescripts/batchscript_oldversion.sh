#!/bin/bash
#SBATCH --job-name=Iteration0ArrayTest      # Job name
#SBATCH --account=gts-pf17                  # Tracking account
#SBATCH --mail-user=avira7@gatech.edu       # E-mail address for notifications
#SBATCH --mail-type=BEGIN,END,FAIL          # Mail preferences
#SBATCH --nodes=4                           # Use one node
#SBATCH --ntasks-per-node=12                # Number of tasks per node
#SBATCH --mem-per-cpu=1gb                   # Memory per processor
#SBATCH --time=02:00:00                     # Time limit hrs:min:sec, max walltime
#SBATCH --output=outputlogs/array_iteration0_test1.out # Standard output and error log
#SBATCH --array=1-5                        # Array range

#Set the number of runs that each SLURM task should do
PER_TASK=1

# Calculate the starting and ending values for this task based
# on the SLURM task and the number of runs per task.
START_NUM=$(( ($SLURM_ARRAY_TASK_ID - 1) * $PER_TASK + 1 ))
END_NUM=$(( $SLURM_ARRAY_TASK_ID * $PER_TASK ))

# Print the task and run range
echo This is task $SLURM_ARRAY_TASK_ID, which will do runs $START_NUM to $END_NUM

# read in ThicknessSweep File
readarray -t a < array_commands 

# Run the loop of runs for this task.
for (( run=$START_NUM; run<=END_NUM; run++ )); do
     echo This is SLURM task $SLURM_ARRAY_TASK_ID, run number $run
     echo ${a[${run-1}]}
     ${a[${run-1}]}
done

date