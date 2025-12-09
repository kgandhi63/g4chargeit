import glob
import h5py
import numpy as np
import uproot
import os
import sys
import gc
import shutil
import time
from concurrent.futures import ProcessPoolExecutor
from functools import partial

# =============================================================================
# 1. DATA READING & PHYSICS LOGIC
# =============================================================================

def read_rootfile(file_path):
    """
    Memory-Optimized Reader: Only loads the columns needed for calculation.
    """
    # Define strict list of columns to save RAM
    needed_branches = [
        "Event_Number", "Particle_Type", "Parent_ID",
        "Volume_Name_Post", "Volume_Name_Pre",
        "Kinetic_Energy_Post_MeV",
        "Pre_Step_Position_mm", "Post_Step_Position_mm"
    ]
   
    try:
        with uproot.open(file_path) as file:
            tree = file["Hit Data"]
            # Load only specific columns
            data = tree.arrays(needed_branches, library="np")
       
        # Stack position arrays (handle nested structure)
        for key in ['Pre_Step_Position_mm', 'Post_Step_Position_mm']:
            data[key] = np.vstack(data[key])
           
        return data
    except Exception as e:
        print(f"Corrupt file or missing tree: {file_path} -> {e}")
        return None

def calculate_stats(data, config="photoemission", volume_name="SiO2"):
    """
    Filters the raw data based on physics configuration.
    """
    if data is None: return {}, {}, {}, {}
   
    N = len(data["Event_Number"])
    if N == 0: return {}, {}, {}, {}

    # Unpack for speed
    event = data["Event_Number"]
    ptype = data["Particle_Type"]
    pid   = data["Parent_ID"]
    vpost = data["Volume_Name_Post"]
    vpre  = data["Volume_Name_Pre"]
    ke_post = data["Kinetic_Energy_Post_MeV"]

    # Helper: Filter unique events
    def filter_unique_indices(indices, keep="first"):
        if len(indices) == 0: return np.array([], dtype=int)
        events_subset = event[indices]
        if keep == "first":
            _, uniq_idx = np.unique(events_subset, return_index=True)
            return indices[uniq_idx]
        else:
            _, uniq_idx = np.unique(events_subset[::-1], return_index=True)
            uniq_idx = len(indices) - 1 - uniq_idx
            return indices[uniq_idx]

    def slice_dict(idx):
        keys = ['Event_Number', 'Particle_Type', 'Pre_Step_Position_mm', 'Post_Step_Position_mm']
        return {k: data[k][idx] for k in keys}

    # --- PHYSICS LOGIC ---
    if "photoemission" in config:
        gamma_idx = np.where((ptype == "gamma") & (pid == 0))[0]
        e2_idx = np.where((ptype == "e-") & (pid > 0))[0]
        e2_last = filter_unique_indices(e2_idx, keep="last")
       
        world_mask = ((vpre[e2_last] == "physical_cyclic") | (vpost[e2_last] == "physical_cyclic"))
        world_e_idx = e2_last[world_mask]
        stopped_idx = np.where((ptype == "e-") & (pid > 0) & (ke_post == 0.0) & (vpost == volume_name))[0]
       
        ejected_events = np.unique(event[world_e_idx])
        created_events = np.unique(event[e2_last])
       
        gamma_to_eject_idx = filter_unique_indices(np.where(np.isin(event, ejected_events))[0], keep="first")
        gamma_to_create_idx = filter_unique_indices(np.where(np.isin(event, created_events))[0], keep="first")
       
        return (slice_dict(gamma_to_create_idx), slice_dict(stopped_idx), slice_dict(gamma_to_eject_idx))

    elif "solarwind" in config:
        proton_inc_mask = (ptype == "proton") & (pid == 0)
        proton_inc_idx = filter_unique_indices(np.where(proton_inc_mask)[0], keep="first")
        last_proton_idx = filter_unique_indices(np.where(ptype == "proton")[0], keep="last")
        protons_inside_idx = last_proton_idx[vpost[last_proton_idx] == volume_name]
       
        e_inc_mask = (ptype == "e-") & (pid == 0)
        e_inc_first = filter_unique_indices(np.where(e_inc_mask)[0], keep="first")
        e_inc_last = filter_unique_indices(np.where(e_inc_mask)[0], keep="last")
        electrons_inside_idx = e_inc_last[vpost[e_inc_last] == volume_name]
       
        return (slice_dict(protons_inside_idx), slice_dict(proton_inc_idx), slice_dict(electrons_inside_idx), slice_dict(e_inc_first))

    # Add "allparticles" logic here if needed...
    return {}

# =============================================================================
# 2. PARALLEL WORKER (WRITES TO TEMP SHARD)
# =============================================================================

def process_shard(file_list, shard_filename, configIN):
    """
    Worker Function: Processes a list of files and writes to a private HDF5 shard.
    """
    print(f"   [Worker] Starting shard: {shard_filename} ({len(file_list)} files)")
   
    with h5py.File(shard_filename, 'w') as h5f:
        # Initialize Groups based on Config
        groups = {}
        if "photoemission" in configIN:
            groups['gamma_initial'] = h5f.create_group('gamma_initial_leading_e_creation')
            groups['e_stopped'] = h5f.create_group('electrons_stopped')
            groups['gamma_ejection'] = h5f.create_group('gamma_initial_leading_to_e_ejection')
        elif "solarwind" in configIN:
            groups['p_stop'] = h5f.create_group('protons_inside')
            groups['p_inc'] = h5f.create_group('protons_incident')
            groups['e_stop'] = h5f.create_group('electrons_inside')
            groups['e_inc'] = h5f.create_group('electrons_incident')

        # Tracking metadata
        collected_iterations = []

        for i, file_path in enumerate(file_list):
            try:
                # Get Iteration Number
                filename = os.path.basename(file_path)
                number_str = filename.split("_")[1]
                iterationNUM = int(''.join(filter(str.isdigit, number_str)))

                # 1. Read
                raw_data = read_rootfile(file_path)
                if raw_data is None: continue

                # 2. Calculate
                data = calculate_stats(raw_data, config=configIN)

                # 3. Write (Using LZF for speed)
                if "photoemission" in configIN:
                    groups['gamma_initial'].create_dataset(f'iter_{iterationNUM}', data=data[0]['Pre_Step_Position_mm'], compression='lzf')
                    groups['e_stopped'].create_dataset(f'iter_{iterationNUM}', data=data[1]['Post_Step_Position_mm'], compression='lzf')
                    groups['gamma_ejection'].create_dataset(f'iter_{iterationNUM}', data=data[2]['Pre_Step_Position_mm'], compression='lzf')
               
                elif "solarwind" in configIN:
                    groups['p_stop'].create_dataset(f'iter_{iterationNUM}', data=data[0]['Post_Step_Position_mm'], compression='lzf')
                    groups['p_inc'].create_dataset(f'iter_{iterationNUM}', data=data[1]['Pre_Step_Position_mm'], compression='lzf')
                    groups['e_stop'].create_dataset(f'iter_{iterationNUM}', data=data[2]['Post_Step_Position_mm'], compression='lzf')
                    groups['e_inc'].create_dataset(f'iter_{iterationNUM}', data=data[3]['Pre_Step_Position_mm'], compression='lzf')

                collected_iterations.append(iterationNUM)
               
                # 4. Cleanup Memory
                del raw_data
                del data
                gc.collect()

            except Exception as e:
                print(f"Error in {filename}: {e}")

        # Save metadata for this shard
        h5f.attrs['iterations'] = collected_iterations
        h5f.attrs['config'] = configIN

    print(f"   [Worker] Finished shard: {shard_filename}")
    return shard_filename

# =============================================================================
# 3. SERIAL MERGER (COMBINES SHARDS)
# =============================================================================

def merge_shards(shard_files, final_output):
    """
    Combines multiple HDF5 files into one.
    Uses 'copy' which is extremely fast (raw byte copy).
    """
    print(f"\n{'='*60}")
    print(f"MERGING {len(shard_files)} SHARDS INTO: {final_output}")
    print(f"{'='*60}")
   
    # Sort input files to keep iterations somewhat ordered (optional)
    shard_files.sort()
   
    # Open the final file
    with h5py.File(final_output, 'w') as h5_master:
       
        # We need to collect all iteration numbers from all files
        all_iterations = []
        master_config = ""

        # Iterate through shards
        for idx, shard in enumerate(shard_files):
            print(f"Merging shard {idx+1}/{len(shard_files)}: {shard} ...")
           
            with h5py.File(shard, 'r') as h5_shard:
                # 1. Metadata
                if idx == 0:
                    master_config = h5_shard.attrs['config']
               
                shard_iters = list(h5_shard.attrs['iterations'])
                all_iterations.extend(shard_iters)
               
                # 2. Copy Datasets
                # Loop over every group (e.g., 'gamma_initial')
                for group_name in h5_shard.keys():
                    # Ensure group exists in master
                    if group_name not in h5_master:
                        h5_master.create_group(group_name)
                   
                    # Loop over every dataset (e.g., 'iter_1', 'iter_5')
                    for dataset_name in h5_shard[group_name].keys():
                        # COPY OPERATION - This is the fast part
                        source_path = f"{group_name}/{dataset_name}"
                        dest_path = f"{group_name}/{dataset_name}"
                       
                        h5_shard.copy(source_path, h5_master[group_name], name=dataset_name)

        # Save Global Metadata
        h5_master.attrs['config'] = master_config
        h5_master.attrs['iterations'] = sorted(all_iterations)
        h5_master.attrs['num_iterations'] = len(all_iterations)
   
    print(f"Merge Complete. Output size: {os.path.getsize(final_output) / (1024*1024):.2f} MB")

# =============================================================================
# 4. MAIN CONTROLLER
# =============================================================================

if __name__ == "__main__":
    # Usage: python script.py <folder> <output_prefix> <config> <max_iter>
    if len(sys.argv) < 4:
        print("Usage: python script.py <folder> <output_file> <config> <max_iter>")
        sys.exit(1)

    folder_path = sys.argv[1]
    output_h5 = sys.argv[2]
    configIN = sys.argv[3]
    max_iteration = int(sys.argv[4]) if len(sys.argv) > 4 else 999999

    directory_path = os.path.join(folder_path, "root/")
    if not output_h5.endswith(".h5"): output_h5 += ".h5"

    # --- 1. Detect Resources ---
    try:
        # Check if running in Slurm
        available_cores = len(os.sched_getaffinity(0))
        print(f"Slurm Allocation Detected: {available_cores} Cores")
    except:
        from multiprocessing import cpu_count
        available_cores = cpu_count()
        print(f"Local System Detected: {available_cores} Cores")

    # Use mostly all cores, but keep a sanity limit for RAM
    # If you have 264GB RAM, using 16-20 workers is safe.
    workers_to_use = min(available_cores, 20)
    print(f"Using {workers_to_use} Parallel Workers.")

    # --- 2. Prepare Files ---
    all_files = sorted(glob.glob(f"{directory_path}/*iteration*{configIN}*.root"))
    filtered_files = []
    for f in all_files:
        try:
            num = int(''.join(filter(str.isdigit, os.path.basename(f).split("_")[1])))
            if num <= max_iteration: filtered_files.append(f)
        except: continue

    total_files = len(filtered_files)
    print(f"Total Files to Process: {total_files}")
   
    # Split files into chunks for the workers
    chunk_size = int(np.ceil(total_files / workers_to_use))
    file_chunks = [filtered_files[i:i + chunk_size] for i in range(0, total_files, chunk_size)]

    # --- 3. PHASE 1: PARALLEL PROCESSING ---
    print(f"\n--- PHASE 1: PARALLEL PROCESSING (Writing {len(file_chunks)} shards) ---")
    start_time = time.time()
   
    temp_files = []
   
    with ProcessPoolExecutor(max_workers=workers_to_use) as executor:
        futures = []
        for i, chunk in enumerate(file_chunks):
            # Create temporary filename: temp_shard_0.h5, etc.
            shard_name = f"temp_shard_{i}.h5"
            temp_files.append(shard_name)
           
            # Submit job
            futures.append(executor.submit(process_shard, chunk, shard_name, configIN))

        # Wait for all to finish
        for future in futures:
            # This will raise an exception if the worker crashed
            res = future.result()

    print(f"Phase 1 Complete in {time.time() - start_time:.2f}s")

    # --- 4. PHASE 2: MERGING ---
    print(f"\n--- PHASE 2: MERGING OUTPUTS ---")
    merge_shards(temp_files, output_h5)

    # --- 5. CLEANUP ---
    print(f"\n--- CLEANUP ---")
    for f in temp_files:
        if os.path.exists(f):
            os.remove(f)
            print(f"Deleted temp file: {f}")

    print(f"\nSUCCESS! Final file saved to: {output_h5}")