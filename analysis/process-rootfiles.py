import glob
import h5py
from multiprocessing import Pool, cpu_count
from functools import partial
import numpy as np
from typing import Tuple, List, Dict, Union
import os
import sys
import gc
import uproot
import math
from concurrent.futures import ProcessPoolExecutor, as_completed, ThreadPoolExecutor


def read_rootfile(file, directory_path=None):
    """
    Read specific columns from ROOT file to save RAM.
    Only loads the branches strictly required by calculate_stats.
    """
    file_path = f"{directory_path}/{file}"
    tree_name = "Hit Data"
 
    # 1. Define ONLY what we need (Drastically reduces RAM usage)
    # Check your ROOT file to ensure these exact names match
    needed_branches = [
        "Event_Number", 
        "Particle_Type", 
        "Parent_ID", 
        "Volume_Name_Post", 
        "Volume_Name_Pre", 
        "Kinetic_Energy_Post_MeV",
        "Pre_Step_Position_mm", 
        "Post_Step_Position_mm"
    ]
 
    with uproot.open(file_path) as file:
        tree = file[tree_name]
        # Only load the specific arrays we need
        branch_vars = tree.arrays(needed_branches, library="np")
 
    # Stack the position arrays (handling the nested structure)
    # Note: uproot.arrays returns a dict-like object, so this works similarly
    for key in ['Pre_Step_Position_mm', 'Post_Step_Position_mm']:
        branch_vars[key] = np.vstack(branch_vars[key])
 
    return branch_vars
 
def process_single_file(fileIN: str, directory_path: str, configIN: str) -> Tuple[int, Union[Tuple, None]]:
    """
    Process a single ROOT file and return iteration number and processed data.
    Memory is freed after processing.
    """
    filename = fileIN.split("/")[-1]
    # print(f"Processing: {filename}")
    # sys.stdout.flush()
    
    # Extract iteration number
    number_str = filename.split("_")[1]
    iterationNUM = int(''.join(filter(str.isdigit, number_str)))
    
    # Read data (returns dict of arrays)
    root_data = read_rootfile(filename, directory_path=directory_path)
    
    # Pass the correctly structured data to calculate_stats
    result = calculate_stats(root_data, config=configIN)
    gc.collect()
    del root_data
 
    # print(f"Finished processing: {filename}")
    # sys.stdout.flush()
    
    return iterationNUM, result
 
def save_to_h5_streaming(data_generator, output_file: str, configIN: str, iterations: List[int]):
    """
    Save processed data to HDF5 file with streaming approach.
    Creates datasets and writes data one iteration at a time.
    
    Args:
        data_generator: Generator yielding (iteration_num, data_tuple)
        output_file: Path to output HDF5 file
        configIN: Configuration type
        iterations: List of iteration numbers for metadata
    """
    with h5py.File(output_file, 'w') as h5f:
        
        # Create groups based on configuration
        if "photoemission" in configIN:
            gamma_group = h5f.create_group('gamma_initial_leading_e_creation')
            e_stopped_group = h5f.create_group('electrons_stopped')
            gamma_ejection_group = h5f.create_group('gamma_initial_leading_to_e_ejection')
            
            for iteration_num, data_tuple in data_generator:
                # Write each dataset immediately
                gamma_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[0]['Pre_Step_Position_mm'],
                    compression='lzf'
                    # compression='gzip',
                    # compression_opts=4
                )
                e_stopped_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[1]['Post_Step_Position_mm'],
                    compression='lzf'
                    # compression='gzip',
                    # compression_opts=4
                )
                gamma_ejection_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[2]['Pre_Step_Position_mm'],
                    compression='lzf'
                    # compression='gzip',
                    # compression_opts=4
                )
                
                # Free memory immediately
                del data_tuple
                gc.collect()
        
        elif "solarwind" in configIN:
            protons_stopped_group = h5f.create_group('protons_inside')
            protons_incident_group = h5f.create_group('protons_incident')
            electrons_stopped_group = h5f.create_group('electrons_inside')
            electrons_incident_group = h5f.create_group('electrons_incident')
            
            for iteration_num, data_tuple in data_generator:
                protons_stopped_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[0]['Post_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
 
                protons_incident_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[1]['Pre_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
 
                electrons_stopped_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[2]['Post_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
 
                electrons_incident_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[3]['Pre_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
                
                del data_tuple
                gc.collect()
        
        elif "allparticles" in configIN:
            gamma_group = h5f.create_group('gamma_initial_leading_e_creation')
            e_stopped_group = h5f.create_group('electrons_stopped_from_gamma')
            protons_group = h5f.create_group('protons_inside')
            electrons_group = h5f.create_group('electrons_inside')
            
            for iteration_num, data_tuple in data_generator:
                gamma_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[0]['Pre_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
                e_stopped_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[1]['Post_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
                protons_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[2]['Post_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
                electrons_group.create_dataset(
                    f'iter_{iteration_num}',
                    data=data_tuple[3]['Post_Step_Position_mm'],
                    compression='lzf'
                    #compression='gzip',
                    #compression_opts=4
                )
                
                del data_tuple
                gc.collect()
        
        # Save metadata
        h5f.attrs['config'] = configIN
        h5f.attrs['num_iterations'] = len(iterations)
        h5f.attrs['iterations'] = iterations
 
def threaded_root_to_h5(configIN: str,
                                directory_path: str,
                                output_h5: str,
                                max_iteration: int = 44,
                                n_threads: int = None):
    """
    Multi-threaded ROOT → HDF5 converter.
    Uses threads on a single node and splits files evenly among them for parallel processing.
    """
 
    if n_threads is None:
        from multiprocessing import cpu_count
        n_threads = cpu_count()
 
    # -------------------------------------------------------
    # 1. Collect and filter files
    # -------------------------------------------------------
    filelist = sorted(glob.glob(f"{directory_path}/*iteration*{configIN}*.root"))
    filtered_files = []
    for fileIN in filelist:
        filename = os.path.basename(fileIN)
        number_str = filename.split("_")[1]
        iterationNUM = int(''.join(filter(str.isdigit, number_str)))
        if iterationNUM <= max_iteration:
            filtered_files.append(fileIN)
 
    total_files = len(filtered_files)
    print(f"Processing {total_files} files using {n_threads} threads on a single node.")
    print("=" * 78)
    sys.stdout.flush()
 
    # -------------------------------------------------------
    # 2. Split files into chunks for each thread
    # -------------------------------------------------------
    chunk_size = math.ceil(total_files / n_threads)
    file_chunks = [filtered_files[i:i + chunk_size] for i in range(0, total_files, chunk_size)]
 
    results = []
 
    # Worker function for a thread
    def worker(file_chunk, thread_id):
        thread_results = []
        for idx, fileIN in enumerate(file_chunk, 1):
            filename = os.path.basename(fileIN)
            iterationNUM, data_tuple = process_single_file(fileIN, directory_path, configIN)
            print(f"[{idx}/{len(file_chunk)}] Thread {thread_id} processed #{iterationNUM}: {filename}")
            sys.stdout.flush()
            thread_results.append((iterationNUM, data_tuple))
        return thread_results
 
    # -------------------------------------------------------
    # 3. Process chunks in parallel threads
    # -------------------------------------------------------
    with ThreadPoolExecutor(max_workers=n_threads) as executor:
        future_results = [executor.submit(worker, chunk, tid)
                          for tid, chunk in enumerate(file_chunks)]
 
        for future in future_results:
            results.extend(future.result())
 
    # -------------------------------------------------------
    # 4. Sort results by iteration number
    # -------------------------------------------------------
    results.sort(key=lambda x: x[0])
    iterations = [r[0] for r in results]
 
    # -------------------------------------------------------
    # 5. Stream to HDF5
    # -------------------------------------------------------
    print("\n" + "=" * 78)
    print("Writing to HDF5 (streaming mode)...")
    sys.stdout.flush()
 
    def result_generator():
        for iteration_num, data_tuple in results:
            yield iteration_num, data_tuple
 
    save_to_h5_streaming(result_generator(), output_h5, configIN, iterations)
 
    print(f"Successfully saved data to: {output_h5}")
    sys.stdout.flush()
 
    del results
    gc.collect()
 
def multiprocessing_root_to_h5_individual(configIN: str,
                                          directory_path: str,
                                          output_h5: str,
                                          max_iteration: int = 44,
                                          n_processes: int = None):
    if n_processes is None:
        from multiprocessing import cpu_count
        n_processes = cpu_count()
 
    # 1. Collect and filter files
    filelist = sorted(glob.glob(f"{directory_path}/*iteration*{configIN}*.root"))
    filtered_files = []
    for fileIN in filelist:
        filename = os.path.basename(fileIN)
        number_str = filename.split("_")[1]
        iterationNUM = int(''.join(filter(str.isdigit, number_str)))
        if iterationNUM <= max_iteration:
            filtered_files.append(fileIN)
 
    total_files = len(filtered_files)
    print(f"Processing {total_files} files using {n_processes} processes on a single node.")
    print("=" * 78)
    sys.stdout.flush()
 
    results = []
 
    # 2. Submit each file individually
    with ProcessPoolExecutor(max_workers=n_processes) as executor:
        future_to_file = {executor.submit(process_single_file, fileIN, directory_path, configIN): fileIN
                          for fileIN in filtered_files}
 
        for idx, future in enumerate(as_completed(future_to_file), 1):
            fileIN = future_to_file[future]
            filename = os.path.basename(fileIN)
            try:
                iterationNUM, data_tuple = future.result()
                print(f"[{idx}/{total_files}] Processed #{iterationNUM}: {filename}")
                sys.stdout.flush()
                results.append((iterationNUM, data_tuple))
            except Exception as e:
                print(f"Error processing {filename}: {e}")
                sys.stdout.flush()
 
    # 3. Sort and stream to HDF5
    results.sort(key=lambda x: x[0])
    iterations = [r[0] for r in results]
 
    print("\n" + "=" * 78)
    print("Writing to HDF5 (streaming mode)...")
    sys.stdout.flush()
 
    def result_generator():
        for iteration_num, data_tuple in results:
            yield iteration_num, data_tuple
 
    save_to_h5_streaming(result_generator(), output_h5, configIN, iterations)
 
    print(f"Saved to {output_h5}")
    sys.stdout.flush()
 
    del results
    gc.collect()
 
 
def serial_root_to_h5(configIN: str,
                                directory_path: str,
                                output_h5: str,
                                max_iteration: int = 44):
    """
    SERIAL version of the ROOT → HDF5 converter.
    Processes files one-by-one with minimal memory usage.
    
    Args:
        configIN: Configuration string ("photoemission", "solarwind", "allparticles")
        directory_path: Directory containing ROOT files
        output_h5: Output HDF5 file path
        max_iteration: Maximum iteration number to process
    """
 
    # -------------------------------------------------------
    # 1. Collect files
    # -------------------------------------------------------
    filelist = sorted(glob.glob(f"{directory_path}/*iteration*{configIN}*.root"))
    
    filtered_files = []
    for fileIN in filelist:
        number_str = fileIN.split("/")[-1].split("_")[1]
        iterationNUM = int(''.join(filter(str.isdigit, number_str)))
        if iterationNUM <= max_iteration:
            filtered_files.append(fileIN)
 
    print(f"Processing {len(filtered_files)} files in SERIAL mode.")
    print("=" * 78)
    sys.stdout.flush()
 
    # -------------------------------------------------------
    # 2. Process in series
    # -------------------------------------------------------
    results = []
    iterations = []
 
    for fileIN in filtered_files:
        filename = fileIN.split("/")[-1]
 
        # Extract iteration number
        number_str = filename.split("_")[1]
        iterationNUM = int(''.join(filter(str.isdigit, number_str)))
 
        # Process file
        data_tuple = process_single_file(fileIN, directory_path, configIN)[1]
        print(f"Processed #{iterationNUM}: {filename}")
        sys.stdout.flush()
 
        results.append((iterationNUM, data_tuple))
        iterations.append(iterationNUM)
 
    # Sort by iteration
    results.sort(key=lambda x: x[0])
    iterations = sorted(iterations)
 
    # -------------------------------------------------------
    # 3. Generator for streaming write
    # -------------------------------------------------------
    def result_generator():
        for iteration_num, data_tuple in results:
            yield iteration_num, data_tuple
 
    # -------------------------------------------------------
    # 4. Stream to HDF5
    # -------------------------------------------------------
    print("\n" + "=" * 78)
    print("Writing to HDF5 (streaming mode)...")
    sys.stdout.flush()
 
    save_to_h5_streaming(result_generator(), output_h5, configIN, iterations)
 
    print(f"Successfully saved data to: {output_h5}")
    sys.stdout.flush()
 
    # Explicit cleanup
    del results
    gc.collect()
 
def read_from_h5(h5_file: str, iteration: int = None) -> Dict:
    """Read data from HDF5 file."""
    with h5py.File(h5_file, 'r') as h5f:
        config = h5f.attrs['config']
        iterations = h5f.attrs['iterations']
 
        data = {
            'iterations': list(iterations),
            'config': config
        }
        
        for group_name in h5f.keys():
            data[group_name] = []
            for iter_num in iterations:
                dataset = h5f[group_name][f'iter_{iter_num}'][()]
                data[group_name].append(dataset)
        
        return data
 
def list_h5_structure(h5_file: str):
    """Print the structure of an HDF5 file."""
    with h5py.File(h5_file, 'r') as h5f:
        print(f"\n{'='*78}")
        print(f"HDF5 File: {h5_file}")
        print(f"{'='*78}")
        print(f"Configuration: {h5f.attrs['config']}")
        print(f"\nGroups (datasets):")
        for group_name in h5f.keys():
            num_datasets = len(h5f[group_name].keys())
            print(f"  - {group_name}: {num_datasets} iterations")
        print(f"{'='*78}\n")
 
def calculate_stats(data, config="photoemission", volume_name="SiO2",printout=False):
    """
    Vectorized statistics for a dictionary of NumPy arrays.
    Returns dictionary slices (each key → filtered array).
    """
 
    N = len(data["Event_Number"])
    if N == 0:
        if "photoemission" in config:
            return {}, {}, {}
        elif "solarwind" in config:
            return {}, {}
        elif "allparticles" in config:
            return {}, {}, {}, {}
        return {}, {}, {}, {}
 
    # shortcuts
    event = data["Event_Number"]
    ptype = data["Particle_Type"]
    pid   = data["Parent_ID"]
    vpost = data["Volume_Name_Post"]
    vpre  = data["Volume_Name_Pre"]
    ke_post = data["Kinetic_Energy_Post_MeV"]
 
    # ---- Helper: unique event filter → event slices ----
    def filter_unique_indices(indices, keep="first"):
        """Return indices uniquely filtered by event number."""
        if len(indices) == 0:
            return np.array([], dtype=int)
 
        events_subset = event[indices]
 
        if keep == "first":
            _, uniq_idx = np.unique(events_subset, return_index=True)
            return indices[uniq_idx]
 
        else:  # keep="last"
            _, uniq_idx = np.unique(events_subset[::-1], return_index=True)
            uniq_idx = len(indices) - 1 - uniq_idx
            return indices[uniq_idx]
 
    # ---- Helper: return dictionary slice ----
    def slice_dict(idx):
 
        select_keys =['Event_Number', 'Particle_Type', 'Pre_Step_Position_mm', 'Post_Step_Position_mm']
        return {k: data[k][idx] for k in select_keys}
 
    # -------------------------------------------------------------------------
    # PHOTOEMISSION
    # -------------------------------------------------------------------------
    # gamma that start the event
    gamma_idx = np.where((ptype == "gamma") & (pid == 0))[0]
    gamma_first = filter_unique_indices(gamma_idx, keep="first")
 
    # secondary electrons
    e2_idx = np.where((ptype == "e-") & (pid > 0))[0]
    e2_last = filter_unique_indices(e2_idx, keep="last")
 
    # world electrons
    world_mask = (
        (vpre[e2_last] == "physical_cyclic") |
        (vpost[e2_last] == "physical_cyclic")
    )
    world_e_idx = e2_last[world_mask]
 
    # stopped electrons
    stopped_idx = np.where(
        (ptype == "e-") &
        (pid > 0) &
        (ke_post == 0.0) &
        (vpost == volume_name)
    )[0]
 
    # event numbers
    ejected_events = np.unique(event[world_e_idx])
    created_events = np.unique(event[e2_last])
 
    gamma_to_eject_idx = filter_unique_indices(
        np.where(np.isin(event, ejected_events))[0], keep="first"
    )
    gamma_to_create_idx = filter_unique_indices(
        np.where(np.isin(event, created_events))[0], keep="first"
    )
 
    # -------------------------------------------------------------------------
    # SOLAR WIND
    # -------------------------------------------------------------------------
    proton_inc_mask = (ptype == "proton") & (pid == 0)
    proton_inc_idx = filter_unique_indices(np.where(proton_inc_mask)[0], keep="first")
 
    last_proton_idx = filter_unique_indices(np.where(ptype == "proton")[0], keep="last")
    protons_inside_idx = last_proton_idx[vpost[last_proton_idx] == volume_name]
 
    e_inc_mask = (ptype == "e-") & (pid == 0)
    e_inc_first = filter_unique_indices(np.where(e_inc_mask)[0], keep="first")
    e_inc_last = filter_unique_indices(np.where(e_inc_mask)[0], keep="last")
    electrons_inside_idx = e_inc_last[vpost[e_inc_last] == volume_name]
 
    # -------------------------------------------------------------------------
    # RETURN SLICED DICTS
    # -------------------------------------------------------------------------
 
      # --- Print outputs and Return ---
    if printout:
        if "photoemission" in config or "allparticles" in config:
            print(f"Photoelectric yield (holes/ γ): {len(gamma_to_create_idx)/len(gamma_first):.4f} "
                f"({len(gamma_to_create_idx)} / {len(gamma_first)})")
            print(f"e- stopped in material: {len(stopped_idx)}")
            print(f"Electrons ejected, from photoelectric effect: {len(gamma_to_eject_idx)}")
 
        if "solarwind" in config or "allparticles" in config:
            print(f"Incident H+ stopped in material: {len(protons_inside_idx)/len(proton_inc_idx):.2%} "
                f"({len(protons_inside_idx)} / {len(proton_inc_idx)})")
            print(f"Incident e- stopped in material: {len(electrons_inside_idx)/len(e_inc_first):.2%} "
                f"({len(electrons_inside_idx)} / {len(e_inc_first)})")
            
    if "photoemission" in config:
        return (
            slice_dict(gamma_to_create_idx),
            slice_dict(stopped_idx),
            slice_dict(gamma_to_eject_idx),
        )
 
    elif "solarwind" in config:
        return (
            slice_dict(protons_inside_idx),
            slice_dict(proton_inc_idx),
            slice_dict(electrons_inside_idx),
            slice_dict(e_inc_first)
        )
    
    elif "allparticles" in config:
        return (
            slice_dict(gamma_to_create_idx),
            slice_dict(stopped_idx),
            slice_dict(protons_inside_idx),
            slice_dict(electrons_inside_idx)
        )
 
    return {}, {}, {}, {}
 
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <folder_path> [note] [config_tag] [max_iteration] [mode]")
        print("\nModes:")
        print("  optimized (default) - Stream results, minimal memory")
        print("  ultra - Process in batches, lowest memory usage")
        print("\nExample:")
        print("  python script.py /path/to/root/files/ my_analysis onlyphotoemission 44 ultra")
        sys.exit(1)
    
    folder_path = sys.argv[1]
    output_h5 = sys.argv[2] if len(sys.argv) > 2 else 'single_config'
    configIN = sys.argv[3] if len(sys.argv) > 3 else 'onlyphotoemission'
    max_iteration = int(sys.argv[4]) if len(sys.argv) > 4 else 44
    
    n_processes = cpu_count() #min(cpu_count(), 6)
    directory_path = os.path.join(folder_path, "root/")
    output_h5 = f"{output_h5}.h5"
    
    print(f"\n{'='*78}")
    print(f"Configuration: {configIN}")
    print(f"Max iteration: {max_iteration}")
    print(f"Directory: {directory_path}")
    print(f"Output file: {output_h5}")
    print(f"Workers: {n_processes}")
 
    # multiprocessing_root_to_h5_individual(
    #     configIN=configIN,
    #     directory_path=directory_path,
    #     output_h5=output_h5,
    #     max_iteration=max_iteration,
    #     n_processes=n_processes,
    # )
 
    # threaded_root_to_h5(
    #     configIN=configIN,
    #     directory_path=directory_path,
    #     output_h5=output_h5,
    #     max_iteration=max_iteration,
    #     n_threads=n_processes,
    # )
 
    serial_root_to_h5(
        configIN=configIN,
        directory_path=directory_path,
        output_h5=output_h5,
        max_iteration=max_iteration,
    )

# example: python process-rootfiles.py "../build" "irregularInitial8max0.8final11_PEparticleloc_until46" 'onlyphotoemission' 46

# #!/bin/bash
# #SBATCH --job-name=process-test
# #SBATCH --account=gts-zjiang33
# #SBATCH --mail-user=avira7@gatech.edu
# #SBATCH --mail-type=BEGIN,END,FAIL
# #SBATCH --nodes=1
# #SBATCH --mem-per-cpu=256gb
# #SBATCH --time=02:00:00
# #SBATCH --output=outputlogs/log-rootprocessing-test

# echo "combine all root files up to a specific iteration number"
# python process-rootfiles.py "../build" "irregularInitial8max0.8final11_PEparticleloc_until46" 'onlyphotoemission' 46
# date