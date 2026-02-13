import os
import sys 
import glob
import numpy as np
import concurrent.futures
import struct 
import sys
from numba import njit, prange 
import h5py
import trimesh
import re
import ast

# --- Full Function Implementations (Optimized Readers/Calculators) ---

@njit(fastmath=True, cache=True)
def scale_fields_numba(field_data):
    """
    Scales the field data array in place by 1e9.
    """
    field_data *= 1e9
    return field_data

@njit(parallel=True)
def compute_E_mag(Ex, Ey, Ez):
    """Computes the E-field magnitude efficiently using Numba."""
    n = Ex.shape[0]
    result = np.empty(n, dtype=Ex.dtype)
    for i in prange(n):
        # We don't need the small epsilon here for magnitude calculation, 
        # but the function remains fast.
        result[i] = np.sqrt(Ex[i]**2 + Ey[i]**2 + Ez[i]**2)
    return result

@njit(parallel=True, fastmath=True, cache=True)
def calculate_filter_mask(pos, target, radius_mm):
    """
    Calculates the mask for points within the radius using Numba, 
    avoiding expensive square root by comparing squared distance to squared radius.
    """
    N = pos.shape[0]
    mask = np.empty(N, dtype=np.bool_)
    
    # Compute squared radius once
    radius_sq = radius_mm * radius_mm
    
    # Use prange for parallel loop
    for i in prange(N):
        # Calculate squared distance (pos is N, 3; target is 3,)
        dx = pos[i, 0] - target[0]
        dy = pos[i, 1] - target[1]
        dz = pos[i, 2] - target[2]
        dist_sq = dx*dx + dy*dy + dz*dz
        
        mask[i] = dist_sq <= radius_sq
        
    return mask


def extract_iterationTime_octreeParameters(file_path):
    """
    Extract the IterationTime and octree parameters from a Geant4 macro file.
    
    Parameters:
    file_path (str): Path to the macro file
    
    Returns:
    dict: Dictionary containing iteration_time and octree parameters, or None values if not found
    """
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Initialize result dictionary with None values
    result = {
        'iteration_time': None,
        'material_temperature': None,
        'minimum_step': None,
        'percent_grad_threshold': None,
        'initial_octree_depth': None,
        'final_octree_depth': None
    }
    
    # Search for IterationTime
    pattern_iteration_time = r'/geometry/IterationTime\s+(\d+\.?\d*(?:[eE][+-]?\d+)?)\s+s'
    match = re.search(pattern_iteration_time, content)
    if match:
        result['iteration_time'] = float(match.group(1))
    
    # Search for MaterialTemperature (with unit conversion)
    pattern_material_temp = r'/geometry/MaterialTemperature\s+(\d+\.?\d*(?:[eE][+-]?\d+)?)\s+(\w+)'
    match = re.search(pattern_material_temp, content)
    if match:
        value = float(match.group(1))
        unit = match.group(2)
        # Convert to Kelvin if needed
        if unit == 'K':
            result['material_temperature'] = value
        elif unit == 'C':
            result['material_temperature'] = value + 273.15
        else:
            result['material_temperature'] = value  # Store as-is if unit unknown
    

    # Search for InitialDepth
    pattern_initial_depth = r'/field/InitialDepth\s+(\d+)'
    match = re.search(pattern_initial_depth, content)
    if match:
        result['initial_octree_depth'] = int(match.group(1))
    
    # Search for MinimumStep (with unit conversion)
    pattern_minimum_step = r'/field/MinimumStep\s+(\d+\.?\d*(?:[eE][+-]?\d+)?)\s+(\w+)'
    match = re.search(pattern_minimum_step, content)
    if match:
        value = float(match.group(1))
        unit = match.group(2)
        # Convert to micrometers if needed
        if unit == 'um':
            result['minimum_step'] = value
        elif unit == 'mm':
            result['minimum_step'] = value * 1000
        elif unit == 'nm':
            result['minimum_step'] = value / 1000
        else:
            result['minimum_step'] = value  # Store as-is if unit unknown
    
    # Search for PercentGradThreshold
    pattern_percent_grad = r'/field/PercentGradThreshold\s+(\d+\.?\d*(?:[eE][+-]?\d+)?)'
    match = re.search(pattern_percent_grad, content)
    if match:
        result['percent_grad_threshold'] = float(match.group(1))
    
    # Search for OctreeDepth
    pattern_octree_depth = r'/field/OctreeDepth\s+(\d+)'
    match = re.search(pattern_octree_depth, content)
    if match:
        result['final_octree_depth'] = int(match.group(1))
    
    return result

def read_adaptive_fieldmap(filename):
    """
    Reads the adaptive field map binary file using a highly optimized
    NumPy approach.
    """
    
    # Define C++ data types for NumPy interpretation
    NODE_DTYPE = np.dtype([
        ('pos', '<f4', 3),     # 3*float32 (center_x, y, z)
        ('field', '<f4', 3),   # 3*float32 (E_x, y, z)
    ])
    
    with open(filename, 'rb') as f:
        
        # Read Octree Parameters (Metadata)
        max_d = np.fromfile(f, dtype=np.uint32, count=1)[0]
        min_s = np.fromfile(f, dtype=np.float64, count=1)[0]
        total_node_count = np.fromfile(f, dtype=np.uint64, count=1)[0]
        final_leaf_count = np.fromfile(f, dtype=np.uint64, count=1)[0] 
        
        # Read ALL Field Nodes Data in one go
        node_raw_array = np.fromfile(f, dtype=NODE_DTYPE, count=total_node_count)

        # Concatenate position and field into a single (N, 6) array
        field_data = np.hstack([
            node_raw_array['pos'],
            node_raw_array['field'],
        ]).astype(np.float32)

        # Construct Metadata
        metadata = {
            'mesh_parameters': {
                'max_depth': max_d,
                'min_step_internal': min_s, 
                'total_nodes': total_node_count,
                'final_leaf_nodes': final_leaf_count
            }
        }
        
        return field_data, metadata

# --- End of Full Function Implementations ---

def process_iteration_radius(filename, target=np.array([-0.1, 0, 0.122]), radius_um=10, scaling=True):
    """
    Worker function to process a single iteration file.
    
    - Reads the field
    - Scales fields if requested
    - Filters points within radius_um from target
    - Extracts iteration time from corresponding macro file
    - Returns iteration key and filtered data dictionary
    """
    # 📢 LOGGING: Print the file being processed
    print(f"   [Worker] Starting file: {os.path.basename(filename)}") 
    sys.stdout.flush() # CRITICAL: Forces immediate printing from the worker process
    
    # Extract iteration number from filename
    try:
        iteration_number = int(os.path.basename(filename).split("-")[0])
    except ValueError:
        iteration_number = os.path.basename(filename)

    # Read data (High Memory Use happens here)
    data, metadata = read_adaptive_fieldmap(filename)
    pos = data[:, :3].astype(np.float32, copy=False)
    Ex = data[:, 3].astype(np.float32, copy=False)
    Ey = data[:, 4].astype(np.float32, copy=False)
    Ez = data[:, 5].astype(np.float32, copy=False)

    if scaling:
        scale_fields_numba(Ex)
        scale_fields_numba(Ey)
        scale_fields_numba(Ez)

    E_mag = compute_E_mag(Ex, Ey, Ez)

    # --- Filter points within radius (now optimized by Numba) ---
    radius_mm = radius_um / 1000
    mask = calculate_filter_mask(pos, target, radius_mm)

    # Create filtered data arrays (NumPy fancy indexing is very fast)
    pos_filtered = pos[mask]
    E_stacked_filtered = np.stack((Ex[mask], Ey[mask], Ez[mask]), axis=1)
    E_mag_filtered = E_mag[mask]
    
    # Clean up memory immediately
    del pos, Ex, Ey, Ez, E_mag, data
    
    filtered_data = {
        "pos": pos_filtered,
        "E": E_stacked_filtered,
        "E_mag": E_mag_filtered,
        "gradRefinements": int(metadata['mesh_parameters']['total_nodes'] - metadata['mesh_parameters']['final_leaf_nodes']),
    }

    return f"iter_{iteration_number}", filtered_data

# --- Parallel driver ---

def process_folder_parallel(folder_path, output_h5="processed-fieldmaps.h5",
                            configIN="onlyphotoemission", target=np.array([-0.1, 0, 0.122]), radius_um=10, max_workers_in=None):
    """
    Process all iteration files in parallel and write results to HDF5.
    
    The max_workers_in argument overrides the default conservative concurrency.
    """
    filenames = sorted(glob.glob(f"{folder_path}/fieldmaps/*{configIN}*.txt"))
    
    if not filenames:
        print(f"⚠️ No files found in '{folder_path}/fieldmaps/' matching pattern '*{configIN}*.txt'. Stopping.")
        return

    # Extract macro parameters from the first macro file (applies to all iterations)
    macro_files = sorted(glob.glob(f"{folder_path}/macros/*{configIN}*.mac"))
    if macro_files:
        macroParameters = extract_iterationTime_octreeParameters(macro_files[1])
        print(f"Extracted macro parameters from: {os.path.basename(macro_files[1])}")
    else:
        print("⚠️ No macro files found. Macro parameters will not be saved.")
        macroParameters = {}

    os.makedirs(os.path.dirname(output_h5), exist_ok=True)

    # 1. Concurrency Control: Use input or hardcoded safe default
    MAX_WORKERS = max_workers_in if max_workers_in else os.cpu_count()
    GZIP_COMPRESSION_LEVEL = 5

    # 📢 LOGGING: Print the worker count
    print(f"Spawning {MAX_WORKERS} workers for {len(filenames)} files.")

    # Create or open the HDF5 file once
    try:
        with h5py.File(output_h5, "w") as h5file:
            
            # Add global metadata attributes to the root of the HDF5 file
            h5file.attrs["central_target_point"] = target  
            h5file.attrs['fieldmap_sphere_um'] = radius_um
            h5file.attrs['config_tag'] = configIN
            
            # Add all macro parameters as global attributes
            for key, value in macroParameters.items():
                if value is not None:
                    h5file.attrs[key] = value
                else:
                    h5file.attrs[key] = -1.0  # Default for None values
            
            print(f"Added global metadata: target={target}, radius={radius_um} µm")
            print(f"Added macro parameters: {macroParameters}")
            
            with concurrent.futures.ProcessPoolExecutor(max_workers=MAX_WORKERS) as executor:
                futures = [executor.submit(process_iteration_radius, fn, target, radius_um) for fn in filenames]

                for future in concurrent.futures.as_completed(futures):
                    try:
                        # Correctly unpack the iteration key and the data dictionary
                        iter_key, filtered_data_dict = future.result()
                        
                        # Get count for logging
                        filtered_count = len(filtered_data_dict['pos'])
                        
                        grp = h5file.create_group(iter_key)
                        
                        # Apply GZIP compression and save data
                        for key, data in filtered_data_dict.items():
                            if key in ["pos", "E", "E_mag"]:
                                # Data arrays get GZIP compression
                                grp.create_dataset(
                                    key, 
                                    data=data, 
                                    compression="gzip", 
                                    compression_opts=GZIP_COMPRESSION_LEVEL
                                )
                            elif key == "gradRefinements":
                                # Save metadata as an attribute
                                grp.attrs[key] = data
                            
                        print(f"✅ Saved {iter_key}: {filtered_count} points (GZIP compressed).")
                        sys.stdout.flush() # CRITICAL: Forces immediate printing from the worker process
                        
                        # Explicitly clear memory after writing to HDF5
                        del filtered_data_dict

                    except Exception as e:
                        # If a worker died, print the error and re-raise
                        print(f"❌ Inner pool exception for an iteration: {e}")
                        raise e

        print(f"\n--- ✅ Completed parallel HDF5 write: {output_h5} ---")
    except Exception as master_error:
        print(f"FATAL ERROR during HDF5 processing: {master_error}")


# --- CLI entry point ---
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("\nUsage: python parallel_fieldmap_processor.py <folder_path> [config_tag] [target_x,y,z] [sphere_radius_um] [geometry_file] [output_name] [max_workers]\n")
        print("Example: python parallel_fieldmap_processor.py ../build-folder onlyphotoemission [-0.1,0,0.137] 50 regularSpheres_fromPython.stl initial_run 4")
        print("\nArguments:")
        print("  folder_path       : Path to the build folder containing fieldmaps/ and macros/")
        print("  config_tag        : Configuration tag to filter files (default: 'onlyphotoemission')")
        print("  target_x,y,z      : Target coordinates in mm, comma-separated (default: [-0.1, 0, 0.137])")
        print("  sphere_radius_um  : Filtering radius in micrometers (default: 50)")
        print("  geometry_file     : STL geometry filename (default: 'regularSpheres_fromPython.stl')")
        print("  output_name       : Output HDF5 filename (without extension, default: 'single_config')")
        print("  max_workers       : Number of parallel workers (default: CPU count)")
        sys.exit(1)

    # Parse arguments in order: folder_path config target sphereRadius outfileName geometryIN maxWorkers
    folder_path = sys.argv[1]
    
    # Argument 2: config_tag
    configIN = sys.argv[2] if len(sys.argv) > 2 else 'onlyphotoemission'

    # Argument 3: target location
    if len(sys.argv) > 3:
        targetIN = np.array(ast.literal_eval(sys.argv[3]), dtype=float)
    else:
        targetIN = np.array([-0.1, 0.0, 0.1 + 0.037])
    
    # Argument 4: sphere_radius_um
    try:
        radiusIN = int(sys.argv[4]) if len(sys.argv) > 4 else 50  # units: µm
    except ValueError:
        print("Error: sphere_radius_um must be an integer.")
        sys.exit(1)
    
    # Argument 6: geometry_file
    geometryIN = sys.argv[5] if len(sys.argv) > 5 else 'regularSpheres_fromPython.stl'

    # Argument 5: output_name
    outputNote = sys.argv[6] if len(sys.argv) > 6 else 'single_config'
    
    # Argument 7: max_workers
    try:
        max_workers_in = int(sys.argv[7]) if len(sys.argv) > 7 else None
    except ValueError:
        print("Error: max_workers must be an integer.")
        sys.exit(1)

    # Load geometry to get the center
    geometry = trimesh.load_mesh(f'{folder_path}/geometry/{geometryIN}') 
    print(f"Imported {geometryIN}, has a center of {geometry.centroid}")
    
    # Add the geometry offset to the target location 
    targetIN = targetIN + geometry.centroid

    # Create output file path
    output_file = f"processed-fieldmaps/{outputNote}.h5"

    print(f"--- Starting Parallel Processing ---")
    print(f"Input Folder: {folder_path}")
    print(f"Config Tag: {configIN}")
    print(f"Geometry File: {geometryIN}")
    print(f"Target Point: {targetIN} µm (relative to geometry centroid: {geometry.centroid})")
    print(f"Filter Radius: {radiusIN} µm")
    print(f"Output File: {output_file}")
    print(f"Max Workers: {max_workers_in if max_workers_in else 'CPU count'}")
    print("------------------------------------")

    process_folder_parallel(
        folder_path, 
        output_h5=output_file, 
        configIN=configIN, 
        target=targetIN, 
        radius_um=radiusIN,
        max_workers_in=max_workers_in
    )

# Example usage:
# python process-fieldmaps.py ../build-folder onlyphotoemission [-0.1,0,0.137] 50 regularSpheres_fromPython.stl initial_run maxWorkers

# SLURM batch script example:
# #!/bin/bash
# #SBATCH --job-name=process-allcases
# #SBATCH --account=gts-zjiang33
# #SBATCH --mail-user=avira7@gatech.edu
# #SBATCH --mail-type=BEGIN,END,FAIL
# #SBATCH --nodes=1
# #SBATCH --mem-per-cpu=264gb
# #SBATCH --time=01:30:00
# #SBATCH --output=outputlogs/log-fieldprocessing-test
#
# source ~/.bashrc
# conda activate geant4-env
#
# echo "Processing fieldmaps for PE case with 10 µm sphere radius"
# python process-fieldmaps.py "../build" 'onlyphotoemission' [-0.1,0,0.137] 10 'regularSpheres_fromPython.stl' 'PEWang_425K_initial8max0.6final10_through111'
# date