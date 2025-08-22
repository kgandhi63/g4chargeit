import os
import re
from collections import defaultdict

# Ensure macro directory exists
os.makedirs("macros", exist_ok=True)

# Define a list of random seeds
seedIN = [10008859, 10005380]

# Output files tracking
output_files = []
i = 0

selected_num = 1000000 # for higher than 1 iterations

def write_macro(f, increment_filename, event_num, input_files=None):
    f.write(f'# Macro file for {increment_filename}\n')
    f.write('#\n')
    f.write('/control/verbose 0\n')
    f.write('/run/verbose 1\n')
    f.write('/event/verbose 0\n')
    f.write('/tracking/verbose 0\n')
    f.write('/process/verbose 0\n')
    f.write('/process/had/verbose 0\n')
    f.write('/process/em/verbose 0\n')
    f.write('#\n')
    f.write(f'/random/setSeeds {seedIN[0]} {seedIN[1]}\n')
    f.write('/random/setSavingFlag 1\n')
    f.write('#\n')
    if input_files:
        f.write('/sphere/rootinput/file ' + ' '.join(input_files) + '\n')
    f.write(f'/sphere/filename root/{increment_filename}.root\n')
    f.write('/sphere/epsilon 3.9\n')
    f.write('/sphere/PBC false\n')
    f.write('#\n')
    f.write('/run/initialize\n')
    f.write('#\n')
    f.write('/gps/particle e-\n')
    f.write('/gps/ene/type Mono\n')
    f.write('/gps/energy 120 eV\n')
    f.write('#\n')
    f.write('/gps/pos/type Plane\n')
    f.write('/gps/pos/shape Square\n')
    f.write('/gps/pos/halfx 50 um\n')
    f.write('/gps/pos/halfy 50 um\n')
    f.write('/gps/pos/centre 0 0 60 um\n')
    f.write('/gps/direction 0 0 -1\n')
    f.write('/gps/ang/type planar\n')
    f.write('#\n')
    f.write('/gps/source/add 1\n')
    f.write('/gps/source/intensity 1.5\n')
    f.write('/gps/particle gamma\n')
    f.write('/gps/ene/type Mono\n')
    f.write('/gps/energy .2 meV\n')
    f.write('/gps/pos/type Plane\n')
    f.write('/gps/pos/shape Square\n')
    f.write('/gps/pos/halfx 50 um\n')
    f.write('/gps/pos/halfy 50 um\n')
    f.write('/gps/pos/centre 0 0 60 um\n')
    f.write('/gps/direction 0 0 -1\n')
    f.write('/gps/ang/type planar\n')
    f.write('#\n')
    f.write('/run/printProgress 10000\n')
    f.write(f'/run/beamOn {event_num}\n')


for incrementIN in [0, 1, 2, 3, 4, 5]:
	if incrementIN == 0:
		event_num_list = [10000, 50000, 100000, 500000, 1000000]
		for event_num in event_num_list:
			increment_filename = f"{i:02d}_iteration0_num{event_num}"
			with open(f"macros/{increment_filename}.mac", 'w') as f:
				write_macro(f, increment_filename, event_num)
			output_files.append((increment_filename + ".root", event_num, incrementIN))
			i += 1

	elif incrementIN == 1:
		# Filter iteration0 files with 100000 particles
		filtered = [fname for fname, evnum, iterID in output_files if "iteration0" in fname]
		for inputfile in filtered:
			increment_filename = f"{i:02d}_iteration1_from_{inputfile.split('_')[0]}_{inputfile.split('_')[-1].replace('.root', '')}"
			with open(f"macros/{increment_filename}.mac", 'w') as f:
				write_macro(f, increment_filename, 100, input_files=[inputfile])
			output_files.append((increment_filename + ".root", 100, incrementIN))
			i += 1

	elif incrementIN >= 2:

		# Filter iteration0 files with 100000 particles
		filtered = [fname for fname, evnum, iterID in output_files if "_num1000000" in fname]
            
		input_list = ' '.join(filtered)

		increment_filename = f"{i:02d}_iteration{incrementIN}_from_num1000000"
		with open(f"macros/{increment_filename}.mac", 'w') as f:
				write_macro(f, increment_filename, 100, input_files=[input_list])
		output_files.append((increment_filename + ".root", 100, incrementIN))
		i += 1
		
#############################################################

# Define paths
macro_dir = "macros"
run_dir = "runscripts"
os.makedirs(run_dir, exist_ok=True)

# Dictionary to hold commands per iteration
iteration_commands = defaultdict(list)

# Pattern to match iteration number
pattern = re.compile(r'iteration(\d+)')

# Collect and categorize macro files
for fname in sorted(os.listdir(macro_dir)):
    if fname.endswith(".mac"):
        match = pattern.search(fname)
        if match:
            iteration_num = int(match.group(1))
            cmd = f"./charging_sphere {os.path.join(macro_dir, fname)}"
            iteration_commands[iteration_num].append(cmd)

# Write out a script per iteration
for iteration_num, commands in iteration_commands.items():
    script_filename = os.path.join(run_dir, f"run_iteration{iteration_num}.sh")
    with open(script_filename, "w") as f:
        f.write("\n".join(commands))
    print(f"Created: {script_filename}")

#############################################################

# Define directories
run_dir = "runscripts"
batch_dir = "batchscripts"
os.makedirs(batch_dir, exist_ok=True)

# Template for SLURM batch file
batch_template_serial = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Run
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=10:00:00
#SBATCH --output=outputlogs/iteration{iter}.out

echo "Starting iteration{iter} runs"
bash {run_script}
date
"""

batch_template_array = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Array
#SBATCH --account=gts-pf17
#SBATCH --mail-user=avira7@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=16
#SBATCH --mem=32gb
#SBATCH --time=10:00:00
#SBATCH --output=outputlogs/iteration{iter}_%A_%a.out
#SBATCH --array=1-{array_size}

# Read in the array of commands
readarray -t commands < {run_script}

# Run the command corresponding to this array task
cmd="${{commands[$SLURM_ARRAY_TASK_ID-1]}}"
echo "Running task $SLURM_ARRAY_TASK_ID: $cmd"
eval "$cmd"

date
"""

# Loop over each run script and create corresponding SLURM script
for fname in os.listdir(run_dir):
    match = re.match(r'run_iteration(\d+)\.sh', fname)
    if match:
        iter_num = int(match.group(1))
        run_script_path = os.path.join(run_dir, fname)
        batch_filename = f"submit_iteration{iter_num}.sh"
        batch_path = os.path.join(batch_dir, batch_filename)

        # Read number of lines in run script for --array
        with open(run_script_path) as f:
            num_lines = sum(1 for _ in f)

        # Use job array for iteration0 and iteration1
        if iter_num in [0, 1]:
            batch_script = batch_template_array.format(
                iter=iter_num,
                run_script=run_script_path,
                array_size=num_lines
            )
        else:
            batch_script = batch_template_serial.format(
                iter=iter_num,
                run_script=run_script_path
            )

        with open(batch_path, "w") as f:
            f.write(batch_script)

        print(f"Created: {batch_path}")

#############################################################

# Directories
batch_dir = "batchscripts"
output_script = "submit_all_iterations.sh"

# Get and sort all batch submission scripts by iteration number
batch_scripts = []
for fname in os.listdir(batch_dir):
    match = re.match(r"submit_iteration(\d+)\.sh", fname)
    if match:
        batch_scripts.append((int(match.group(1)), fname))

batch_scripts.sort()  # Sort by iteration number

# Start writing the master submission script
with open(output_script, "w") as f:
    f.write("#!/bin/bash\n\n")

    previous_job_var = ""
    for i, (_, script_name) in enumerate(batch_scripts):
        script_path = os.path.join(batch_dir, script_name)
        if i == 0:
            f.write(f"# Submit the first job and capture its Job ID\n")
            f.write(f"jid0=$(sbatch --parsable {script_path})\n")
            f.write(f"echo \"Submitted iteration 0 job: $jid0\"\n\n")
            previous_job_var = "$jid0"
        else:
            f.write(f"# Submit job {i} with dependency on previous job\n")
            f.write(f"jid{i}=$(sbatch --dependency=afterok:{previous_job_var} {script_path})\n")
            f.write(f"echo \"Submitted iteration {i} job (afterok dependency): $jid{i}\"\n\n")
            previous_job_var = f"$jid{i}"

# Make the script executable
os.chmod("submit_all_iterations.sh", 0o755)
print(f"Created chained submission script: {output_script}")