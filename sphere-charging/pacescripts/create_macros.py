import os
import re
import glob
import shutil
import numpy as np
from collections import defaultdict

# Overwrite the "macros" folder
if os.path.exists("macros"):
    shutil.rmtree("macros")  # Delete the folder and all its contents

os.makedirs("macros", exist_ok=True)  # Recreate it cleanly

# define the number of particles for the each iteration
account = "zjiang33"
username = "kgandhi63"
eventnumbers_onlysolarwind = 2000 # adjusted this number to reflect the timestep in Zimmerman manuscript
eventnumbers_onlyphotoemission = 5000 # adjusted this number to reflect the timestep in Zimmerman manuscript
eventnumbers_allparticles = 10000 # adjusted this number to reflect the timestep in Zimmerman manuscript
iterationNUM = 50 # number of iterations to perform
# be careful here, there is a userlimit for the number of jobs that can be submited (around 500)

# list of configurations
config_list = ["onlysolarwind", "onlyphotoemission", "allparticles"] #["onlysolarwind", "onlyphotoemission", "allparticles"]

# define the size of the world
CAD_dimensions = (200,600,546.410) # in units of microns
particle_position = 30 # place all particles 200 microns above the geometry
bufferXY = 0 # in units of microns
worldX = CAD_dimensions[0] + 2*bufferXY # in units of microns -- account for angle of beam
worldY = CAD_dimensions[1] + 2*bufferXY # in units of microns
worldZ = CAD_dimensions[2] + 2*particle_position # in units of microns

# calculate offset for SW ions at 45 degrees
Z_position = CAD_dimensions[2]/2 + particle_position
XY_offset = Z_position - CAD_dimensions[2]/2 + bufferXY
rotation = np.sin(45*np.pi/180)
#worldX += XY_offset # increase to account for offset in X direction

# Output files tracking
output_files = []
seedIN = [10008859, 10005380]
i = 0

def write_macro(f, increment_filename, event_num, input_files=None):
    f.write(f'# Macro file for {increment_filename}\n')
    f.write('#\n')
    f.write('/control/verbose 0\n')
    f.write('/run/verbose 0\n')
    f.write('/event/verbose 0\n')
    f.write('/tracking/verbose 0\n')
    f.write('/process/verbose 0\n')
    f.write('/process/had/verbose 0\n')
    f.write('/process/em/verbose 0\n')
    #f.write('/cuts/setLowEdge 5 eV\n')
    #f.write('/run/setCut 5 nm')
    f.write('#\n')
    f.write(f'/random/setSeeds {seedIN[0]} {seedIN[1]}\n')
    f.write('/random/setSavingFlag 1\n')
    f.write('#\n')
    f.write('/process/em/fluo true\n')
    f.write('/process/em/auger true\n')
    f.write('/process/em/augerCascade true\n')
    f.write('/process/em/deexcitationIgnoreCut true\n')
    f.write('/process/em/Polarisation true\n')
    f.write('/process/em/PhotoeffectBelowKShell true\n')
    f.write('#\n')
    f.write('/process/em/lowestElectronEnergy 0 eV\n')
    f.write('/process/em/lowestMuHadEnergy 10 eV\n')
    f.write('/process/em/enableSamplingTable 1\n')
    f.write('#\n')
    f.write('/process/eLoss/CSDARange 1\n')
    f.write('/process/eLoss/UseICRU90 1\n')
    f.write('/process/eLoss/UseAngularGenerator true\n')
    f.write('#\n')
    f.write(f'/sphere/worldX {worldX} um\n')
    f.write(f'/sphere/worldY {worldY} um\n')
    f.write(f'/sphere/worldZ {worldZ} um\n')
    f.write(f'/sphere/fieldMapStep 10 um\n') # step size for field map solver
    f.write('#\n')
    if input_files:
        f.write('/sphere/rootinput/file ' + ' '.join(input_files) + '\n')
    f.write(f'/sphere/filename root/{increment_filename}.root\n')
    f.write('/sphere/cadinput/file stacked_spheres_frompython.stl\n')
    #f.write('/sphere/cadinput/scale 0.001\n')
    f.write('/sphere/epsilon 4\n') 
    f.write('/sphere/PBC true\n')
    f.write('/run/initialize\n')
    f.write('#\n')
    # only include the SW plasma (ions and electrons)
    if "onlysolarwind" in increment_filename:
        f.write('/gps/particle proton\n')
        f.write('/gps/ene/type Mono\n')
        f.write('/gps/energy 1 keV\n') 
        f.write('/gps/pos/type Plane\n')
        f.write('/gps/pos/shape Square\n')
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
        f.write(f'/gps/direction -{rotation} 0 -{rotation} \n')
        f.write('#\n')
        f.write('/gps/source/add 1\n')
        f.write("/gps/particle e-\n")
        f.write("/gps/source/intensity 2\n")
        f.write("/gps/ene/type Arb\n")
        f.write("/gps/hist/type arb\n")
        f.write("/gps/ene/diffspec true\n")
        f.write("/gps/hist/file electron_distribution.txt\n")
        f.write("/gps/hist/inter Lin\n")
        f.write("/gps/pos/type Plane\n")
        f.write("/gps/pos/shape Square\n")
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f"/gps/pos/centre 0 0 {Z_position} um\n")
        f.write("/gps/ang/type iso\n")
        f.write('#\n') 
    # include only the photons   
    elif "onlyphotoemission" in increment_filename:
        f.write('/gps/particle gamma\n')
        f.write('/gps/pos/type Plane\n')
        f.write('/gps/pos/shape Square\n')
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
        f.write(f'/gps/direction -{rotation} 0 -{rotation} \n')
        f.write("/gps/ene/type Arb\n")
        f.write("/gps/hist/type arb\n")
        f.write("/gps/ene/diffspec true\n")
        f.write("/gps/hist/file photon_distribution.txt\n")
        f.write("/gps/hist/inter Lin\n")
        # f.write('/gps/ene/type Mono\n')
        # f.write('/gps/energy 10 eV\n')
        #f.write('/gps/direction 0 0 -1\n')
    # all particles (photons and SW ions and electrons)
    elif "allparticles" in increment_filename:
        f.write('/gps/particle proton\n')
        f.write('/gps/ene/type Mono\n')
        f.write('/gps/energy 1 keV\n') 
        f.write('/gps/pos/type Plane\n')
        f.write('/gps/pos/shape Square\n')
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
        f.write(f'/gps/direction -{rotation} 0 -{rotation} \n')
        f.write('#\n')
        f.write('/gps/source/add 1\n')
        f.write("/gps/particle e-\n")
        f.write("/gps/source/intensity 2\n")
        f.write("/gps/ene/type Arb\n")
        f.write("/gps/hist/type arb\n")
        f.write("/gps/ene/diffspec true\n")
        f.write("/gps/hist/file electron_distribution.txt\n")
        f.write("/gps/hist/inter Lin\n")
        f.write("/gps/pos/type Plane\n")
        f.write("/gps/pos/shape Square\n")
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f"/gps/pos/centre 0 0 {Z_position} um\n")
        f.write("/gps/ang/type iso\n")
        f.write('#\n')
        f.write('/gps/source/add 2\n')
        f.write('/gps/source/intensity 15 \n') # trial and error to determine this intensity based on desired bulk concentration
        f.write('/gps/particle gamma\n')
        f.write('/gps/pos/type Plane\n')
        f.write('/gps/pos/shape Square\n')
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
        f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
        f.write(f'/gps/direction -{rotation} 0 -{rotation} \n')
        f.write("/gps/ene/type Arb\n")
        f.write("/gps/hist/type arb\n")
        f.write("/gps/ene/diffspec true\n")
        f.write("/gps/hist/file photon_distribution.txt\n")
        f.write("/gps/hist/inter Lin\n")
    else: 
        Warning("need to select from: onlysolarwind, onlyphotoemission, allparticles")
    f.write('#\n')
    f.write('/run/printProgress 100\n')
    f.write(f'/run/beamOn {event_num}\n')

#############################################################

output_files = []
i = 0

for optionIN in config_list:

    select_num = vars()['eventnumbers_' + optionIN] # selected_numSW, selected_numPE, selected_numall

    for incrementIN in range(iterationNUM):

        if incrementIN == 0:
            # Iteration 0 with selected_num
            increment_filename = f"{i:03d}_stackediteration0_{optionIN}_num{select_num}"
            with open(f"macros/{increment_filename}.mac", 'w') as f:
                write_macro(f, increment_filename, select_num)
            output_files.append((increment_filename + ".root", select_num, incrementIN, optionIN))
            i += 1

        elif incrementIN > 0:
            # Iteration 2+ using only iteration 1 files with 1000000 particles
            #filtered = [fname for fname, evnum, iterID in output_files if iterID ==0 or evnum == selected_num]
            filtered = [fname for fname, evnum, iterID, option in output_files if option == optionIN] # or (evnum == selected_num)

            input_list = ' '.join(filtered)
            #print(input_list)
    
            increment_filename = f"{i:03d}_stackediteration{incrementIN}_{optionIN}_from_00_num{select_num}"
            with open(f"macros/{increment_filename}.mac", 'w') as f:
                write_macro(f, increment_filename, select_num, input_files=[input_list])
            output_files.append((increment_filename + ".root", select_num, incrementIN, optionIN))
            i += 1


for optionIN in config_list:

    select_num, i = 1000000, 0

    increment_filename = f"{i:03d}_stackediteration0_{optionIN}_num{select_num}"
    with open(f"macros/{increment_filename}.mac", 'w') as f:
        write_macro(f, increment_filename, select_num)
    output_files.append((increment_filename + ".root", select_num, 0, optionIN))

#############################################################

# Define directories
batch_dir = "batchscripts"
if os.path.exists(batch_dir):
    shutil.rmtree(batch_dir)  # Delete the folder and all its contents

os.makedirs(batch_dir, exist_ok=True)  # Recreate it cleanly

# Template for SLURM batch file
batch_template = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Configuration{config}
#SBATCH --account=gts-{account}
#SBATCH --mail-user={username}@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=18:00:00
#SBATCH --output=outputlogs/iteration{iter}_{config}_%A

module load openmpi/4.1.5

echo "Starting iteration{iter} for {config} configuration"
srun {run_line}
date
"""
#bash {run_script}

batch_template_array = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Array
#SBATCH --account=gts-{account}
#SBATCH --mail-user={username}@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=32gb
#SBATCH --time=15:00:00
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

# Define paths
macro_dir = "macros"

# Pattern to match iteration number
pattern = re.compile(r'iteration(\d+)')

# loop over all configurations
i=0
for configIN in config_list:

    # Collect and categorize macro files
    for fname in sorted(os.listdir(macro_dir)):
        if configIN in fname and fname.endswith(".mac"):
            match = pattern.search(fname)
            if match:
                iteration_num = int(match.group(1))
                cmd = f"./charging_sphere {os.path.join(macro_dir, fname)}"
                batch_script = batch_template.format(
                    iter=iteration_num,
                    config=configIN,
                    run_line=cmd,                
                    account=account, 
                    username=username
                )
                #iteration_commands[iteration_num].append(cmd)

                batch_filename = f"{i:03d}_submit_iteration{iteration_num}_for{configIN}.sh"
                batch_path = os.path.join(batch_dir, batch_filename)
                with open(batch_path, "w") as f:
                    f.write(batch_script)

                i+=1

print(f"Created all batchscripts: 0 through {i-1}")

#############################################################

# Directories
batch_dir = "batchscripts"
output_script = "submit_all_iterations.sh"

# Get and sort all batch script files
batch_scripts = glob.glob(os.path.join(batch_dir, "*.sh"))
batch_scripts.sort()  # Sort alphabetically (can be adjusted if needed)

# Start writing the master submission script
with open(output_script, "w") as f:
    f.write("#!/bin/bash\n\n")

    previous_job_var = ""
    for i, script_path in enumerate(batch_scripts):

        # Use regex to find the iteration number
        match = re.search(r"iteration(\d+)_for", script_path)
        if match:
            iteration = int(match.group(1))

        if iteration == 0:
            f.write("# Submit the first job and capture its Job ID\n")
            f.write(f"jid{i}=$(sbatch --parsable {script_path})\n")
            f.write(f"echo \"Submitted job for runscript: $jid{i}\"\n\n")
            previous_job_var = f"$jid{i}"
        else:
            f.write(f"# Submit job {i} with dependency on previous job\n")
            f.write(f"jid{i}=$(sbatch --parsable --dependency=afterok:{previous_job_var} {script_path})\n")
            f.write(f"echo \"Submitted job for runscript (afterok dependency): $jid{i}\"\n\n")
            previous_job_var = f"$jid{i}"

# Make the script executable
os.chmod("submit_all_iterations.sh", 0o755)
print(f"Created chained submission script: {output_script}")
