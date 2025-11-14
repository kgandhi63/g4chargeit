import os
import re
import glob
import shutil
import numpy as np
import pandas as pd
from collections import defaultdict

import uproot
import numpy as np

#############################################################
###################### USER PARAMETERS ######################
#############################################################

# define the number of particles for the each iteration
account, username = "pf17", "avira7"
eventnumbers_onlysolarwind = 100000 # adjusted this number to reflect the timestep in Zimmerman manuscript
eventnumbers_onlyphotoemission = 100000 # adjusted this number to reflect the timestep in Zimmerman manuscript
eventnumbers_allparticles = 10000 # adjusted this number to reflect the timestep in Zimmerman manuscript
iterationNUM = 200 # number of iterations to perform
temperature = 425 # temperature for dissipation model, units: kelvin
density = 2.20 # density of SiO2, units: g/cm3
#seedIN = [10008859, 10005380] # for debugging purposes

# list of configurations
config_list = ["onlysolarwind", "onlyphotoemission"]#["onlysolarwind", "onlyphotoemission", "allparticles"]
minStepList = [0.1, 0.05] # minimum step for Octree mesh for each case (units of um)
initialOctreeDepth = 8
gradPercent = 0.8
finalOctreeDepth = 12
chargeDissipation = "true"

# define the size of the world
CAD_dimensions = (400, 300, 373.2) # in units of microns
particle_position = 15 # place all particles microns above the geometry
buffer_plane = 20 # in units of microns
bufferXY = 0 # in units of microns
worldX = CAD_dimensions[0] + 2*bufferXY # in units of microns -- account for angle of beam
worldY = CAD_dimensions[1] + 2*bufferXY # in units of microns
worldZ = CAD_dimensions[2] + 2*particle_position # in units of microns

# calculate offset for SW ions at 45 degrees
Z_position = CAD_dimensions[2]/2 + particle_position
XY_offset = Z_position - CAD_dimensions[2]/2 + bufferXY
rotation = np.sin(45*np.pi/180)

# flux values from Zimmerman 2016 manuscript
PEflux, SWelectrons, SWions = np.array([4e-6, 1.5e-6, 3e-7])*6.241509e18 #units: A/m2 -> e/m2/s
planeArea = worldX*worldY/(1e6**2) # units: m2

# Template for SLURM batch file
batch_template = """#!/bin/bash
#SBATCH --job-name=Iteration{iter}_Configuration{config}
#SBATCH --account=gts-{account}
#SBATCH --mail-user={username}@gatech.edu
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --mem-per-cpu=8gb
#SBATCH --time=05:00:00
#SBATCH --output=outputlogs/%A_iteration{iter}_{config}

echo "Starting iteration{iter} for {config} configuration"
{run_line}
date
"""

#############################################################
###################### MACRO CODE ###########################
#############################################################

def reset_folder(folder_path):
    if os.path.exists(folder_path):
        shutil.rmtree(folder_path)
    os.makedirs(folder_path, exist_ok=True)

# Reset both folders
reset_folder("macros")
reset_folder("batchscripts")

# Output files tracking
output_files = []
i = 0

def write_macro(f, increment_filename, event_num, iterationTime, input_files=None, minStep=1):
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
    # The variable exists, now check its value or just execute the code
    if 'seedIN' in locals() or 'seedIN' in globals():
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
    f.write('/process/em/lowestMuHadEnergy 100 eV\n')
    f.write('/process/em/enableSamplingTable 1\n')
    f.write('#\n')
    f.write('/process/eLoss/CSDARange 1\n')
    f.write('/process/eLoss/UseICRU90 1\n')
    f.write('/process/eLoss/UseAngularGenerator true\n')
    f.write('#\n')
    f.write(f'/sphere/worldX {worldX} um\n')
    f.write(f'/sphere/worldY {worldY} um\n')
    f.write(f'/sphere/worldZ {worldZ} um\n')
    f.write(f'/sphere/MaterialTemperature {temperature} K\n')
    f.write(f'/sphere/MaterialDensity {density} g/cm3\n')
    f.write(f'/sphere/IterationTime {iterationTime} s\n')
    f.write(f'/sphere/ApplyChargeDissipation {chargeDissipation}\n')
    f.write(f'/sphere/field/InitialDepth {initialOctreeDepth}\n')
    f.write(f'/sphere/field/MinimumStep {minStep} um\n') # step size for field map solver
    f.write(f'/sphere/field/PercentGradThreshold {gradPercent}\n') # step size for field map solver
    f.write(f'/sphere/field/OctreeDepth {finalOctreeDepth}\n') # step size for field map solver
    f.write(f'/sphere/field/file fieldmaps/{increment_filename.split("_")[0]}-{increment_filename.split("_")[2]}-fieldmap.txt \n')
    f.write(f'/sphere/charges/file charges-{increment_filename.split("_")[2]}.txt\n')
    f.write('#\n')
    if input_files:
        f.write('/sphere/rootinput/file ' + ' '.join(input_files) + '\n')
    f.write(f'/sphere/rootoutput/file root/{increment_filename}.root\n')
    f.write('/sphere/cadinput/file stacked_spheres_frompython_cropped.stl\n')
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
        f.write(f'/gps/pos/halfx {CAD_dimensions[0]/2+buffer_plane} um\n')
        f.write(f'/gps/pos/halfy {CAD_dimensions[1]/2+buffer_plane} um\n')
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
    f.write('/run/printProgress 5000\n')
    f.write(f'/run/beamOn {event_num}\n')

def read_rootfile(file,directory_path=None):
    #file = '00_iteration0_num5000.root' #'13_iteration5_from_num1000000.root'

    if directory_path:
        file_path = f"{directory_path}/{file}"
    else:
        file_path = file
    tree_name = "Hit Data"

    with uproot.open(file_path) as file:
        
        tree = file[tree_name]
        branch_names = tree.keys()
        branch_vars = {}
        for name in branch_names:
            branch_vars[name] = tree[name].array(library="np")

    df = pd.DataFrame(branch_vars)

    return df 

def get_particle_counts_by_type(root_file, directory_path=None):
    try:
        df = read_rootfile(root_file, directory_path)
        counts = {}
        unique_particles = df["Particle_Type"].unique()
        for particleIN in unique_particles:

            if particleIN == "gamma":
                # get dataframe of the last e- event within the sensitive detector
                last_e_event = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] > 0.0)].drop_duplicates(subset="Event_Number", keep="last")
                # get new dataframe of all e- that left the world
                world_e_energy=last_e_event[(last_e_event["Volume_Name_Post"]=="physical_cyclic") | (last_e_event["Volume_Name_Pre"]=="physical_cyclic")]
                # get all the initial gamma energy that leads to an e- escaping
                matching_event_numbers = np.intersect1d(df["Event_Number"], world_e_energy["Event_Number"])

                #  get all the initial gamma energy that leds to the creation of e-
                matching_event_numbers = np.intersect1d(df["Event_Number"], last_e_event["Event_Number"])
                gamma_initial_leading_e_creation = df[df["Event_Number"].isin(matching_event_numbers)].drop_duplicates(subset="Event_Number", keep="first")
                counts[particleIN] = gamma_initial_leading_e_creation.shape[0]
            else:
                counts[particleIN] = df[(df["Particle_Type"] == particleIN)& (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first").shape[0]
        
        return counts

    except Exception as e:
        print(f"Error reading {root_file}: {e}")
        return {}


#############################################################
############### CREATE ITERATIONS ###########################
#############################################################

output_files = []

for optionIN,minStepIN in zip(config_list, minStepList):

    select_num = vars()['eventnumbers_' + optionIN]
    i = 0  # Reset iteration counter for each config

    while i < iterationNUM:

        if i == 0:
            # Create macro for iteration 0
            increment_filename = f"{i:03d}_iteration0_{optionIN}_num{select_num}"
            macro_path = f"macros/{increment_filename}.mac"
            with open(macro_path, 'w') as f:
                write_macro(f, increment_filename, select_num, 0, minStep=minStepIN)
            output_files.append((increment_filename + ".root", select_num, i, optionIN))

            # Create batch script
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
            # Wait for and read ROOT file from iteration 0
            previous_root_file = f"root/{output_files[-1][0]}"

            if not os.path.exists(previous_root_file):
                print(f"[{optionIN}] requires to advance: {previous_root_file}")
                break  # STOP iteration for this config until ROOT exists

            particlesforIteration = get_particle_counts_by_type(previous_root_file)

            if particlesforIteration == 0:
                print(f"[{optionIN}] No particles found in {previous_root_file}")
                break  # STOP and wait for valid ROOT file

            # Calculate iteration time
            if optionIN == "onlyphotoemission":
                iterationTime = (particlesforIteration["gamma"] / planeArea) / PEflux
            elif optionIN == "onlysolarwind":
                iterationTime = (particlesforIteration["proton"] / planeArea) / SWions
            else:
                iterationTime = 0

            print(f"[{optionIN}] Particle counts: {particlesforIteration} Iteration time (s): {iterationTime}")

            increment_filename = f"{i:03d}_iteration{i}_{optionIN}_num{select_num}"
            macro_path = f"macros/{increment_filename}.mac"
            with open(macro_path, 'w') as f:
                write_macro(f, increment_filename, select_num, iterationTime, #(i+1)
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
            #filtered = [fname for fname, _, _, option in output_files if option == optionIN]
            #input_list = ' '.join(filtered)
            previous_root_file = f"root/{output_files[-1][0]}"

            increment_filename = f"{i:03d}_iteration{i}_{optionIN}_num{select_num}"
            macro_path = f"macros/{increment_filename}.mac"
            with open(macro_path, 'w') as f:
                write_macro(f, increment_filename, select_num, iterationTime, #*(i+1)
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

print(f"Created batchscripts for iterations: 0 through {i}")

# for optionIN in config_list:

#     select_num, i = 1000000, 0

#     increment_filename = f"{i:03d}_stackediteration0_{optionIN}_num{select_num}"
#     with open(f"macros/{increment_filename}.mac", 'w') as f:
#         write_macro(f, increment_filename, select_num)
#     output_files.append((increment_filename + ".root", select_num, 0, optionIN))

#############################################################
#############################################################
