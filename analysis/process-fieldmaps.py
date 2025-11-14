import os
import sys 
import glob
import numpy as np
import concurrent.futures
import struct 
import sys
from numba import njit, prange 
import h5py

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
        "gradRefinements": int(metadata['mesh_parameters']['total_nodes'] - metadata['mesh_parameters']['final_leaf_nodes'])
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

    os.makedirs(os.path.dirname(output_h5), exist_ok=True)

    # 1. Concurrency Control: Use input or hardcoded safe default (4)
    MAX_WORKERS = os.cpu_count() #max(1, (os.cpu_count() or 2)//2)
    GZIP_COMPRESSION_LEVEL = 5

    # 📢 LOGGING: Print the worker count
    print(f"Spawning {MAX_WORKERS} workers for {len(filenames)} files.")

    # Create or open the HDF5 file once
    try:
        with h5py.File(output_h5, "w") as h5file:
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
        print("\nUsage: python parallel_fieldmap_processor.py <folder_path> [note] [config_tag] [temp_K] [max_workers]\n")
        print("Example: python parallel_fieldmap_processor.py ../build-folder initial_run onlyphotoemission 425 4")
        sys.exit(1)

    # 1. Parse arguments
    # python script.py <folder_path> [note] [config_tag] [saving_radius] [max_workers]
    folder_path = sys.argv[1]
    noteIN = sys.argv[2] if len(sys.argv) > 2 else 'single_config'
    configIN = sys.argv[3] if len(sys.argv) > 3 else 'onlyphotoemission' 
    radiusIN = int(sys.argv[4]) if len(sys.argv) > 4 else 100 # units: µm
    
    # New argument: max_workers
    try:
        max_workers_in = int(sys.argv[5]) if len(sys.argv) > 5 else None
    except ValueError:
        print("Error: max_workers must be an integer.")
        sys.exit(1)

    # Fixed target and radius parameters (moved to main execution setup)
    targetIN = np.array([-0.1, 0, 0.1 - 0.015 + 0.037])
    #targetIN = np.array([0.1, 0, 0.1 - 0.015 + 0.037])
    tempIN = 425  # units: K

    output_file = f"processed-fieldmaps/{noteIN}.h5"

    print(f"--- Starting Parallel Processing ---")
    print(f"Input Folder: {folder_path}")
    print(f"Output File: {output_file}")
    print(f"Config Tag: {configIN}")
    print(f"Target Radius: {radiusIN} µm centered at {targetIN}")
    print("------------------------------------")

    process_folder_parallel(
        folder_path, 
        output_h5=output_file, 
        configIN=configIN, 
        target=targetIN, 
        radius_um=radiusIN,
        max_workers_in=max_workers_in # Pass the new argument
    )

# example: python process-fieldmaps.py  "../build-smallerworld-initial8max0.8final12" 'initial8max0.8final12' 'onlyphotoemission' 