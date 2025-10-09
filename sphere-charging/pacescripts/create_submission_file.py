import os
import re
import glob
import shutil
import numpy as np
import sys
from collections import defaultdict

# Default configuration
DEFAULT_JOB_TYPES = "photo+solar"

# Get command line arguments
if len(sys.argv) < 3:
    print("Usage: python create_submission_file.py <start_iteration> <end_iteration> [job_types]")
    print("Example: python create_submission_file.py 5 10")
    print("Example: python create_submission_file.py 5 10 photoemission")
    print("Example: python create_submission_file.py 5 10 solarwind") 
    print("Example: python create_submission_file.py 5 10 allparticles")
    print("Example: python create_submission_file.py 5 10 photo+solar")
    print("Example: python create_submission_file.py 5 10 allcases")
    print("job_types: photoemission, solarwind, allparticles, photo+solar, or allcases (default: photo+solar)")
    sys.exit(1)

try:
    start_iteration = int(sys.argv[1])
    end_iteration = int(sys.argv[2])
except ValueError:
    print("Error: Start and end iterations must be integers")
    sys.exit(1)

# Get job types from command line or use default
if len(sys.argv) >= 4:
    job_types = sys.argv[3].lower()
    if job_types not in ['photoemission', 'solarwind', 'allparticles', 'photo+solar', 'allcases']:
        print(f"Error: Invalid job type '{job_types}'. Must be 'photoemission', 'solarwind', 'allparticles', 'photo+solar', or 'allcases'")
        sys.exit(1)
else:
    job_types = DEFAULT_JOB_TYPES

# Directories
batch_dir = "batchscripts"
output_script = "submit_iterations.sh"

# Get and sort all batch script files
batch_scripts = glob.glob(os.path.join(batch_dir, "*.sh"))
batch_scripts.sort()  # Sort alphabetically (can be adjusted if needed)

# Group scripts by type
photoemission_scripts = []
solarwind_scripts = []
allparticles_scripts = []

for script_path in batch_scripts:
    # Use regex to find the iteration number and type
    match = re.search(r"iteration(\d+)_for(onlyphotoemission|onlysolarwind|allparticles)", script_path)
    if match:
        iteration = int(match.group(1))
        script_type = match.group(2)
        
        # Skip scripts outside our desired range
        if iteration < start_iteration or iteration > end_iteration:
            continue
            
        if script_type == "onlyphotoemission":
            photoemission_scripts.append((iteration, script_path))
        elif script_type == "onlysolarwind":
            solarwind_scripts.append((iteration, script_path))
        elif script_type == "allparticles":
            allparticles_scripts.append((iteration, script_path))

# Sort by iteration
photoemission_scripts.sort(key=lambda x: x[0])
solarwind_scripts.sort(key=lambda x: x[0])
allparticles_scripts.sort(key=lambda x: x[0])

# Filter job types based on user selection
include_photoemission = job_types in ['photoemission', 'photo+solar', 'allcases']
include_solarwind = job_types in ['solarwind', 'photo+solar', 'allcases']
include_allparticles = job_types in ['allparticles', 'allcases']

# Start writing the master submission script
with open(output_script, "w") as f:
    f.write("#!/bin/bash\n\n")
    f.write(f"# Submission script for iterations {start_iteration} to {end_iteration}\n")
    f.write(f"# Job types: {job_types}\n\n")

    job_count = 0
    
    # Submit photoemission jobs with dependencies (if selected)
    if include_photoemission and photoemission_scripts:
        f.write("# Photoemission jobs\n")
        previous_photo_job = ""
        
        for i, (iteration, script_path) in enumerate(photoemission_scripts):
            if i == 0:
                f.write(f"# Submit first photoemission job (iteration {iteration})\n")
                f.write(f"jid{job_count}=$(sbatch --parsable {script_path})\n")
                f.write(f"echo \"Submitted photoemission job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_photo_job = f"$jid{job_count}"
            else:
                f.write(f"# Submit photoemission job for iteration {iteration} with dependency\n")
                f.write(f"jid{job_count}=$(sbatch --parsable --dependency=afterok:{previous_photo_job} {script_path})\n")
                f.write(f"echo \"Submitted photoemission job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_photo_job = f"$jid{job_count}"
            
            job_count += 1
    elif include_photoemission and not photoemission_scripts:
        f.write("# No photoemission jobs found in the specified range\n\n")
    
    # Submit solarwind jobs with dependencies (if selected)
    if include_solarwind and solarwind_scripts:
        f.write("# Solarwind jobs\n")
        previous_solar_job = ""
        
        for i, (iteration, script_path) in enumerate(solarwind_scripts):
            if i == 0:
                f.write(f"# Submit first solarwind job (iteration {iteration})\n")
                f.write(f"jid{job_count}=$(sbatch --parsable {script_path})\n")
                f.write(f"echo \"Submitted solarwind job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_solar_job = f"$jid{job_count}"
            else:
                f.write(f"# Submit solarwind job for iteration {iteration} with dependency\n")
                f.write(f"jid{job_count}=$(sbatch --parsable --dependency=afterok:{previous_solar_job} {script_path})\n")
                f.write(f"echo \"Submitted solarwind job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_solar_job = f"$jid{job_count}"
            
            job_count += 1
    elif include_solarwind and not solarwind_scripts:
        f.write("# No solarwind jobs found in the specified range\n\n")

    # Submit allparticles jobs with dependencies (if selected)
    if include_allparticles and allparticles_scripts:
        f.write("# Allparticles jobs\n")
        previous_allparticles_job = ""
        
        for i, (iteration, script_path) in enumerate(allparticles_scripts):
            if i == 0:
                f.write(f"# Submit first allparticles job (iteration {iteration})\n")
                f.write(f"jid{job_count}=$(sbatch --parsable {script_path})\n")
                f.write(f"echo \"Submitted allparticles job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_allparticles_job = f"$jid{job_count}"
            else:
                f.write(f"# Submit allparticles job for iteration {iteration} with dependency\n")
                f.write(f"jid{job_count}=$(sbatch --parsable --dependency=afterok:{previous_allparticles_job} {script_path})\n")
                f.write(f"echo \"Submitted allparticles job for iteration {iteration}: $jid{job_count}\"\n\n")
                previous_allparticles_job = f"$jid{job_count}"
            
            job_count += 1
    elif include_allparticles and not allparticles_scripts:
        f.write("# No allparticles jobs found in the specified range\n\n")

    if job_count == 0:
        f.write(f"echo \"No batch scripts found for iterations {start_iteration} to {end_iteration}\"\n")
        print(f"Warning: No batch scripts found for iterations {start_iteration} to {end_iteration}")
    else:
        print(f"Created submission script with {job_count} jobs for iterations {start_iteration} to {end_iteration}")
        if include_photoemission:
            print(f" - {len(photoemission_scripts)} photoemission jobs")
        if include_solarwind:
            print(f" - {len(solarwind_scripts)} solarwind jobs")
        if include_allparticles:
            print(f" - {len(allparticles_scripts)} allparticles jobs")

# Make the script executable
os.chmod("submit_iterations.sh", 0o755)