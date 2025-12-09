import os
import re
import glob
import shutil
import numpy as np
import pandas as pd
from collections import defaultdict
import uproot

#############################################################
###################### USER PARAMETERS ######################
#############################################################

# Account and user settings
account, username = "zjiang33", "avira7"

# Material properties
density = 1.9  # g/cm3 (from Wang manuscript)
temperature = 425  # K (293 K for room temp in Wang experiments)

# Iteration settings
iterationNUM = 1
# seedIN = [10008859, 10005380]  # Optional: set random seed for debugging

# Experimental parameters from Wang 2016 manuscript
electron_energy = 120  # eV
photonSource_wavelength = 172  # nm
photonSource_FWHM = 14  # nm
photonSource_center = 1239.8 / photonSource_wavelength  # eV
photonSource_sigma = abs((1239.8/(photonSource_wavelength + photonSource_FWHM/2) - 
                         1239.8/(photonSource_wavelength - photonSource_FWHM/2)) / 2.3548)  # eV

# UV photon energies to simulate
photonSource_energy = [5.3] #, 5.4, 5.5, 5.6, 6, 8, 10, 12, 13]  # eV

# Configuration list
configList = ["onlyphotoemission", "onlyUV"]

# Flux parameters from Wang et al. 2016 (experimental setup)
UVflux, eflux = np.array([5.05e-6, 2*3e-7]) * 6.241509e18  # A/m2 -> e/m2/s

# Flux parameters from Zimmerman et al. 2016 (lunar environment)
PEflux, SWelectrons, SWions = np.array([4e-6, 1.5e-6, 3e-7]) * 6.241509e18  # A/m2 -> e/m2/s

# Mesh and simulation parameters
minStepList = [0.01, 0.1]  # Minimum step for Octree mesh (µm)
eventnumbersList = [10000000, 10000000]
# eventnumbersList = [50000, 50000]  # For testing
initialOctreeDepth = 8
gradPercent = 0.7
finalOctreeDepth = 11
chargeDissipation = "true"

# Geometry and CAD settings
CAD_filename = "isolated_grains_interpolated.stl"  # STL file to import
CAD_dimensions = (107.00044, 120.77438, 100.29045)  # µm
particle_position = 15  # µm above geometry
bufferXY = 0  # µm
worldX = CAD_dimensions[0] + 2*bufferXY  # µm
worldY = CAD_dimensions[1] + 2*bufferXY  # µm
worldZ = CAD_dimensions[2] + 2*particle_position  # µm
planeArea = worldX * worldY / (1e6**2)  # m2

# Calculate offset for SW ions at 45 degrees
Z_position = CAD_dimensions[2]/2 + particle_position
XY_offset = Z_position - CAD_dimensions[2]/2 + bufferXY
rotation = np.sin(45 * np.pi / 180)

# Template for SLURM batch file
batch_template = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Configuration{config}
#SBATCH --account=gts-{account}
#SBATCH --mail-user={username}@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --mem-per-cpu=4gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/%A_iteration{iter}_{config}

echo "Starting iteration{iter} for {config} configuration"
{run_line}
date
"""

#############################################################
###################### HELPER FUNCTIONS #####################
#############################################################

def reset_folder(folder_path):
    """Remove and recreate a folder."""
    if os.path.exists(folder_path):
        shutil.rmtree(folder_path)
    os.makedirs(folder_path, exist_ok=True)


def read_rootfile(file, directory_path=None):
    """
    Read a ROOT file and return a pandas DataFrame.
    
    Parameters:
    -----------
    file : str
        ROOT filename
    directory_path : str, optional
        Directory containing the ROOT file
    
    Returns:
    --------
    pd.DataFrame
        Data from the ROOT file
    """
    file_path = f"{directory_path}/{file}" if directory_path else file
    tree_name = "Hit Data"

    with uproot.open(file_path) as root_file:
        tree = root_file[tree_name]
        branch_names = tree.keys()
        branch_vars = {name: tree[name].array(library="np") for name in branch_names}

    return pd.DataFrame(branch_vars)


def get_particle_counts_by_type(root_file, directory_path=None):
    """
    Extract particle counts by type from a ROOT file.
    
    Parameters:
    -----------
    root_file : str
        ROOT filename
    directory_path : str, optional
        Directory containing the ROOT file
    
    Returns:
    --------
    dict
        Particle type -> count mapping
    """
    try:
        df = read_rootfile(root_file, directory_path)
        counts = {}
        unique_particles = df["Particle_Type"].unique()
        
        for particle in unique_particles:
            counts[particle] = df[
                (df["Particle_Type"] == particle) & 
                (df["Parent_ID"] == 0.0)
            ].drop_duplicates(subset="Event_Number", keep="first").shape[0]
        
        return counts

    except Exception as e:
        print(f"Error reading {root_file}: {e}")
        return {}


def write_macro_header(f, increment_filename):
    """Write common macro file header."""
    f.write(f'# Macro file for {increment_filename}\n')
    f.write('#\n')
    f.write('/control/verbose 0\n')
    f.write('/run/verbose 0\n')
    f.write('/event/verbose 0\n')
    f.write('/tracking/verbose 0\n')
    f.write('/process/verbose 0\n')
    f.write('/process/had/verbose 0\n')
    f.write('/process/em/verbose 0\n')
    f.write('#\n')
    
    # Optional random seed
    if 'seedIN' in globals():
        f.write(f'/random/setSeeds {seedIN[0]} {seedIN[1]}\n')
        f.write('/random/setSavingFlag 1\n')
    f.write('#\n')


def write_physics_settings(f):
    """Write physics process settings."""
    f.write('/process/em/fluo true\n')
    f.write('/process/em/auger true\n')
    f.write('/process/em/augerCascade true\n')
    f.write('/process/em/deexcitationIgnoreCut true\n')
    f.write('/process/em/Polarisation true\n')
    f.write('/process/em/PhotoeffectBelowKShell true\n')
    f.write('#\n')
    f.write('/process/em/lowestElectronEnergy 0 eV\n')
    f.write('/process/em/lowestMuHadEnergy 100 eV\n')
    f.write('/process/em/enableSamplingTable 1\n')
    f.write('/process/em/binsPerDecade 40\n')
    f.write('#\n')
    f.write('/process/eLoss/CSDARange 1\n')
    f.write('/process/eLoss/UseICRU90 1\n')
    f.write('/process/eLoss/UseAngularGenerator true\n')
    f.write('/process/eLoss/StepFunction 0.2 0.001\n')
    f.write('#\n')


def write_geometry_settings(f, increment_filename, iterationTime, minStep, input_files=None):
    """Write geometry and field settings."""
    f.write(f'/sphere/worldX {worldX} um\n')
    f.write(f'/sphere/worldY {worldY} um\n')
    f.write(f'/sphere/worldZ {worldZ} um\n')
    f.write(f'/sphere/MaterialTemperature {temperature} K\n')
    f.write(f'/sphere/MaterialDensity {density} g/cm3\n')
    f.write(f'/sphere/IterationTime {iterationTime} s\n')
    f.write(f'/sphere/ApplyChargeDissipation {chargeDissipation}\n')
    f.write(f'/sphere/field/InitialDepth {initialOctreeDepth}\n')
    f.write(f'/sphere/field/MinimumStep {minStep} um\n')
    f.write(f'/sphere/field/PercentGradThreshold {gradPercent}\n')
    f.write(f'/sphere/field/OctreeDepth {finalOctreeDepth}\n')
    
    # Extract base name for field map file
    base_name = f"{increment_filename.split('_')[0]}-{increment_filename.split('_')[2]}"
    f.write(f'/sphere/field/file fieldmaps/{base_name}-fieldmap.txt\n')
    f.write(f'/sphere/charges/file charges-{increment_filename.split("_")[2]}.txt\n')
    f.write('#\n')
    
    # Input files
    if input_files:
        f.write('/sphere/rootinput/file ' + ' '.join(input_files) + '\n')
    
    f.write(f'/sphere/rootoutput/file root/{increment_filename}.root\n')
    f.write(f'/sphere/cadinput/file {CAD_filename}\n')
    f.write('/sphere/epsilon 4\n')
    f.write('/sphere/PBC true\n')
    f.write('/run/initialize\n')
    f.write('#\n')


def write_particle_source_electrons(f):
    """Write GPS commands for electron source."""
    f.write("/gps/particle e-\n")
    f.write('/gps/ene/type Mono\n')
    f.write(f'/gps/energy {electron_energy} eV\n')
    f.write('/gps/pos/type Plane\n')
    f.write('/gps/pos/shape Square\n')
    f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
    f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
    f.write(f"/gps/pos/centre 0 0 {Z_position} um\n")
    f.write("/gps/ang/type iso\n")
    f.write('#\n')


def write_particle_source_uv_single(f, energy):
    """Write GPS commands for single UV energy."""
    f.write('/gps/particle gamma\n')
    f.write(f'/gps/ene/mono {energy} eV\n')
    f.write("/gps/pos/type Plane\n")
    f.write("/gps/pos/shape Square\n")
    f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
    f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
    f.write(f"/gps/pos/centre 0 0 {Z_position} um\n")
    f.write('/gps/direction 0 0 -1\n')
    f.write("/gps/ang/type cos\n")


def write_particle_source_uv_multiple(f):
    """Write GPS commands for multiple UV energies."""
    for energy in photonSource_energy:
        write_particle_source_uv_single(f, energy)


def write_particle_source_solarwind(f):
    """Write GPS commands for solar wind (protons + electrons)."""
    # Protons
    f.write('/gps/particle proton\n')
    f.write('/gps/ene/type Mono\n')
    f.write('/gps/energy 1 keV\n')
    f.write('/gps/pos/type Plane\n')
    f.write('/gps/pos/shape Square\n')
    f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
    f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
    f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
    f.write(f'/gps/direction -{rotation} 0 -{rotation}\n')
    f.write('#\n')
    
    # Electrons (Maxwellian distribution)
    f.write('/gps/source/add 1\n')
    f.write("/gps/particle e-\n")
    f.write("/gps/source/intensity 5\n")
    f.write("/gps/ene/type Arb\n")
    f.write("/gps/hist/type arb\n")
    f.write("/gps/ene/diffspec true\n")
    f.write("/gps/hist/file electronMaxwellian_distribution.txt\n")
    f.write("/gps/hist/inter Lin\n")
    f.write("/gps/pos/type Plane\n")
    f.write("/gps/pos/shape Square\n")
    f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
    f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
    f.write(f"/gps/pos/centre 0 0 {Z_position} um\n")
    f.write("/gps/ang/type iso\n")
    f.write('#\n')


def write_particle_source_photoemission(f):
    """Write GPS commands for photoemission (solar spectrum)."""
    f.write('/gps/particle gamma\n')
    f.write('/gps/pos/type Plane\n')
    f.write('/gps/pos/shape Square\n')
    f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2} um\n')
    f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2} um\n')
    f.write(f'/gps/pos/centre {XY_offset} 0 {Z_position} um\n')
    f.write(f'/gps/direction -{rotation} 0 -{rotation}\n')
    f.write("/gps/ene/type Arb\n")
    f.write("/gps/hist/type arb\n")
    f.write("/gps/ene/diffspec true\n")
    f.write("/gps/hist/file photonSolar_distribution.txt\n")
    f.write("/gps/hist/inter Lin\n")


def write_macro(f, increment_filename, event_num, iterationTime, input_files=None, minStep=1):
    """
    Write complete macro file.
    
    Parameters:
    -----------
    f : file object
        Open file to write to
    increment_filename : str
        Name for this macro/output
    event_num : int
        Number of events to simulate
    iterationTime : float
        Physical time for this iteration (seconds)
    input_files : list, optional
        Previous ROOT files to read
    minStep : float
        Minimum step size for octree mesh (µm)
    """
    write_macro_header(f, increment_filename)
    write_physics_settings(f)
    write_geometry_settings(f, increment_filename, iterationTime, minStep, input_files)
    
    # Select particle source based on configuration
    if "onlyelectrons" in increment_filename:
        write_particle_source_electrons(f)
    elif "onlyUV" in increment_filename:
        # Check if this is a single-energy UV simulation
        energy_match = re.search(r'energy([\d.]+)eV', increment_filename)
        if energy_match:
            energy = float(energy_match.group(1))
            write_particle_source_uv_single(f, energy)
        else:
            write_particle_source_uv_multiple(f)
    elif "onlysolarwind" in increment_filename:
        write_particle_source_solarwind(f)
    elif "onlyphotoemission" in increment_filename:
        write_particle_source_photoemission(f)
    else:
        raise Warning("Configuration must be one of: onlyUV, onlyelectrons, onlysolarwind, onlyphotoemission")
    
    f.write('#\n')
    f.write('/run/printProgress 5000\n')
    f.write(f'/run/beamOn {event_num}\n')


#############################################################
############### CREATE ITERATIONS ###########################
#############################################################

# Reset output folders
reset_folder("macros")
reset_folder("batchscripts")

output_files = []

for optionIN, minStepIN, select_num in zip(configList, minStepList, eventnumbersList):
    
    # For onlyUV, create separate macros for each energy
    if optionIN == "onlyUV":
        for energy in photonSource_energy:
            config_name = f"{optionIN}-energy{energy}eV"
            i = 0  # Reset iteration counter
            
            while i < iterationNUM:
                
                if i == 0:
                    # Create macro for iteration 0
                    increment_filename = f"{i:03d}_iteration0_{config_name}_num{select_num}"
                    macro_path = f"macros/{increment_filename}.mac"
                    
                    with open(macro_path, 'w') as f:
                        write_macro(f, increment_filename, select_num, 0, minStep=minStepIN)
                    
                    output_files.append((increment_filename + ".root", select_num, i, config_name))
                    
                    # Create batch script
                    batch_path = f"batchscripts/{i:03d}_submit_iteration0_{config_name}.sh"
                    cmd = f"./charging_sphere {macro_path}"
                    
                    with open(batch_path, "w") as f:
                        batch_script = batch_template.format(
                            iter=i,
                            config=config_name,
                            run_line=cmd,
                            account=account,
                            username=username
                        )
                        f.write(batch_script)
                    
                    i += 1
                
                elif i == 1:
                    # Read ROOT file from iteration 0
                    previous_root_file = f"root/{output_files[-1][0]}"
                    
                    if not os.path.exists(previous_root_file):
                        print(f"[{config_name}] Requires: {previous_root_file}")
                        break
                    
                    particlesforIteration = get_particle_counts_by_type(previous_root_file)
                    
                    if not particlesforIteration:
                        print(f"[{config_name}] No particles found in {previous_root_file}")
                        break
                    
                    # Calculate iteration time
                    iterationTime = (particlesforIteration.get("gamma", 0) / planeArea) / UVflux
                    
                    print(f"[{config_name}] Particles: {particlesforIteration}, Time: {iterationTime:.2e} s")
                    
                    increment_filename = f"{i:03d}_iteration{i}_{config_name}_num{select_num}"
                    macro_path = f"macros/{increment_filename}.mac"
                    
                    with open(macro_path, 'w') as f:
                        write_macro(f, increment_filename, select_num, iterationTime,
                                  input_files=[f"{output_files[-1][0]}"], minStep=minStepIN)
                    
                    output_files.append((increment_filename + ".root", select_num, i, config_name))
                    
                    # Create batch script
                    batch_path = f"batchscripts/{i:03d}_submit_iteration{i}_{config_name}.sh"
                    cmd = f"./charging_sphere {macro_path}"
                    
                    with open(batch_path, "w") as f:
                        batch_script = batch_template.format(
                            iter=i,
                            config=config_name,
                            run_line=cmd,
                            account=account,
                            username=username
                        )
                        f.write(batch_script)
                    
                    i += 1
                
                else:  # i > 1
                    increment_filename = f"{i:03d}_iteration{i}_{config_name}_num{select_num}"
                    macro_path = f"macros/{increment_filename}.mac"
                    
                    with open(macro_path, 'w') as f:
                        write_macro(f, increment_filename, select_num, iterationTime,
                                  input_files=[f"{output_files[-1][0]}"], minStep=minStepIN)
                    
                    output_files.append((increment_filename + ".root", select_num, i, config_name))
                    
                    # Create batch script
                    batch_path = f"batchscripts/{i:03d}_submit_iteration{i}_{config_name}.sh"
                    cmd = f"./charging_sphere {macro_path}"
                    
                    with open(batch_path, "w") as f:
                        batch_script = batch_template.format(
                            iter=i,
                            config=config_name,
                            run_line=cmd,
                            account=account,
                            username=username
                        )
                        f.write(batch_script)
                    
                    i += 1
    
    else:
        # Standard processing for other configurations
        i = 0
        
        while i < iterationNUM:
            
            if i == 0:
                increment_filename = f"{i:03d}_iteration0_{optionIN}_num{select_num}"
                macro_path = f"macros/{increment_filename}.mac"
                
                with open(macro_path, 'w') as f:
                    write_macro(f, increment_filename, select_num, 0, minStep=minStepIN)
                
                output_files.append((increment_filename + ".root", select_num, i, optionIN))
                
                batch_path = f"batchscripts/{i:03d}_submit_iteration0_{optionIN}.sh"
                cmd = f"./charging_sphere {macro_path}"
                
                with open(batch_path, "w") as f:
                    batch_script = batch_template.format(
                        iter=i,
                        config=optionIN,
                        run_line=cmd,
                        account=account,
                        username=username
                    )
                    f.write(batch_script)
                
                i += 1
            
            elif i == 1:
                previous_root_file = f"root/{output_files[-1][0]}"
                
                if not os.path.exists(previous_root_file):
                    print(f"[{optionIN}] Requires: {previous_root_file}")
                    break
                
                particlesforIteration = get_particle_counts_by_type(previous_root_file)
                
                if not particlesforIteration:
                    print(f"[{optionIN}] No particles found in {previous_root_file}")
                    break
                
                # Calculate iteration time based on configuration
                if optionIN == "onlyphotoemission":
                    iterationTime = (particlesforIteration.get("gamma", 0) / planeArea) / PEflux
                elif optionIN == "onlysolarwind":
                    iterationTime = (particlesforIteration.get("proton", 0) / planeArea) / SWions
                elif optionIN == "onlyelectrons":
                    iterationTime = (particlesforIteration.get("e-", 0) / planeArea) / eflux
                else:
                    iterationTime = 0
                
                print(f"[{optionIN}] Particles: {particlesforIteration}, Time: {iterationTime:.2e} s")
                
                increment_filename = f"{i:03d}_iteration{i}_{optionIN}_num{select_num}"
                macro_path = f"macros/{increment_filename}.mac"
                
                with open(macro_path, 'w') as f:
                    write_macro(f, increment_filename, select_num, iterationTime,
                              input_files=[f"{output_files[-1][0]}"], minStep=minStepIN)
                
                output_files.append((increment_filename + ".root", select_num, i, optionIN))
                
                batch_path = f"batchscripts/{i:03d}_submit_iteration{i}_{optionIN}.sh"
                cmd = f"./charging_sphere {macro_path}"
                
                with open(batch_path, "w") as f:
                    batch_script = batch_template.format(
                        iter=i,
                        config=optionIN,
                        run_line=cmd,
                        account=account,
                        username=username
                    )
                    f.write(batch_script)
                
                i += 1
            
            else:  # i > 1
                increment_filename = f"{i:03d}_iteration{i}_{optionIN}_num{select_num}"
                macro_path = f"macros/{increment_filename}.mac"
                
                with open(macro_path, 'w') as f:
                    write_macro(f, increment_filename, select_num, iterationTime,
                              input_files=[f"{output_files[-1][0]}"], minStep=minStepIN)
                
                output_files.append((increment_filename + ".root", select_num, i, optionIN))
                
                batch_path = f"batchscripts/{i:03d}_submit_iteration{i}_{optionIN}.sh"
                cmd = f"./charging_sphere {macro_path}"
                
                with open(batch_path, "w") as f:
                    batch_script = batch_template.format(
                        iter=i,
                        config=optionIN,
                        run_line=cmd,
                        account=account,
                        username=username
                    )
                    f.write(batch_script)
                
                i += 1

print(f"\nCreated {len(output_files)} macro files and batch scripts")
print(f"Configurations: {set([item[3] for item in output_files])}")