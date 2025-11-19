import glob
import h5py
import pandas as pd
from multiprocessing import Pool, cpu_count
from functools import partial
import numpy as np
from typing import Tuple, List, Dict, Union
import os
import sys

from common_functions import *

def process_single_file(fileIN: str, directory_path: str, configIN: str) -> Tuple[int, Union[Tuple, None]]:
    """
    Process a single ROOT file and return iteration number and dataframes.
    
    Args:
        fileIN: Full path to ROOT file
        directory_path: Directory containing ROOT files
        configIN: Configuration string (photoemission, solarwind, allparticles)
    
    Returns:
        Tuple of (iteration_number, tuple_of_dataframes)
        - photoemission: (gamma_initial_leading_e_creation, electrons_stopped, gamma_initial_leading_to_e_ejection)
        - solarwind: (protons_inside, electrons_inside)
        - allparticles: (gamma_initial_leading_e_creation, electrons_stopped, protons_inside, electrons_inside)
    """
    filename = fileIN.split("/")[-1]
    print(f"Processing: {filename}")
    sys.stdout.flush() # CRITICAL: Forces immediate printing from the worker process
    
    # Extract iteration number
    number_str = filename.split("_")[1]
    iterationNUM = int(''.join(filter(str.isdigit, number_str)))
    
    # Read and process data
    result = calculate_stats(
        read_rootfile(filename, directory_path=directory_path), 
        config=configIN
    )
    
    return iterationNUM, result


def save_to_h5(data_dict: Dict, output_file: str, configIN: str):
    """
    Save processed data to HDF5 file with proper structure based on configuration.
    
    Args:
        data_dict: Dictionary containing processed dataframes and metadata
        output_file: Path to output HDF5 file
        configIN: Configuration type
    """
    with h5py.File(output_file, 'w') as h5f:
        
        if "photoemission" in configIN:
            # Save gamma_initial_leading_e_creation
            gamma_group = h5f.create_group('gamma_initial_leading_e_creation')
            for idx, df in enumerate(data_dict['gamma_initial_leading_e_creation']):
                iteration_num = data_dict['iterations'][idx]
                gamma_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save electrons_stopped
            e_stopped_group = h5f.create_group('electrons_stopped')
            for idx, df in enumerate(data_dict['electrons_stopped']):
                iteration_num = data_dict['iterations'][idx]
                e_stopped_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save gamma_initial_leading_to_e_ejection
            gamma_ejection_group = h5f.create_group('gamma_initial_leading_to_e_ejection')
            for idx, df in enumerate(data_dict['gamma_initial_leading_to_e_ejection']):
                iteration_num = data_dict['iterations'][idx]
                gamma_ejection_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
        
        elif "solarwind" in configIN:
            # Save protons_inside
            protons_group = h5f.create_group('protons_inside')
            for idx, df in enumerate(data_dict['protons_inside']):
                iteration_num = data_dict['iterations'][idx]
                protons_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save electrons_inside
            electrons_group = h5f.create_group('electrons_inside')
            for idx, df in enumerate(data_dict['electrons_inside']):
                iteration_num = data_dict['iterations'][idx]
                electrons_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
        
        elif "allparticles" in configIN:
            # Save gamma_initial_leading_e_creation
            gamma_group = h5f.create_group('gamma_initial_leading_e_creation')
            for idx, df in enumerate(data_dict['gamma_initial_leading_e_creation']):
                iteration_num = data_dict['iterations'][idx]
                gamma_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save electrons_stopped
            e_stopped_group = h5f.create_group('electrons_stopped')
            for idx, df in enumerate(data_dict['electrons_stopped']):
                iteration_num = data_dict['iterations'][idx]
                e_stopped_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save protons_inside
            protons_group = h5f.create_group('protons_inside')
            for idx, df in enumerate(data_dict['protons_inside']):
                iteration_num = data_dict['iterations'][idx]
                protons_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
            
            # Save electrons_inside
            electrons_group = h5f.create_group('electrons_inside')
            for idx, df in enumerate(data_dict['electrons_inside']):
                iteration_num = data_dict['iterations'][idx]
                electrons_group.create_dataset(
                    f'iteration_{iteration_num}',
                    data=df.to_records(index=False),
                    compression='gzip',
                    compression_opts=4
                )
        
        # Save metadata
        h5f.attrs['config'] = configIN
        h5f.attrs['num_iterations'] = len(data_dict['iterations'])
        h5f.attrs['iterations'] = data_dict['iterations']


def parallel_root_to_h5(configIN: str, 
                        directory_path: str, 
                        output_h5: str,
                        max_iteration: int = 44,
                        n_processes: int = None):
    """
    Read ROOT files in parallel and save to HDF5.
    
    Args:
        configIN: Configuration string ("photoemission", "solarwind", "allparticles")
        directory_path: Directory containing ROOT files
        output_h5: Output HDF5 file path
        max_iteration: Maximum iteration number to process
        n_processes: Number of parallel processes (None = use all CPUs)
    
    Returns:
        Dictionary containing all processed data
    """
    # Get file list
    filelist = sorted(glob.glob(f"{directory_path}/*iteration*{configIN}*_num100000.root"))
    
    # Filter files by iteration number
    filtered_files = []
    for fileIN in filelist:
        number_str = fileIN.split("/")[-1].split("_")[1]
        iterationNUM = int(''.join(filter(str.isdigit, number_str)))
        if iterationNUM <= max_iteration:
            filtered_files.append(fileIN)
    
    print(f"Processing {len(filtered_files)} files with {n_processes or cpu_count()} processes")
    print("=" * 78)
    sys.stdout.flush() # CRITICAL: Forces immediate printing from the worker process
    
    # Process files in parallel
    if n_processes is None:
        n_processes = cpu_count()
    
    process_func = partial(process_single_file, 
                          directory_path=directory_path, 
                          configIN=configIN)
    
    with Pool(processes=n_processes) as pool:
        results = pool.map(process_func, filtered_files)
    
    # Sort results by iteration number
    results.sort(key=lambda x: x[0])
    
    # Organize data based on configuration
    data_dict = {
        'iterations': [r[0] for r in results],
        'config': configIN
    }
    
    if "photoemission" in configIN:
        data_dict['gamma_initial_leading_e_creation'] = [r[1][0] for r in results]
        data_dict['electrons_stopped'] = [r[1][1] for r in results]
        data_dict['gamma_initial_leading_to_e_ejection'] = [r[1][2] for r in results]
    
    elif "solarwind" in configIN:
        data_dict['protons_inside'] = [r[1][0] for r in results]
        data_dict['electrons_inside'] = [r[1][1] for r in results]
    
    elif "allparticles" in configIN:
        data_dict['gamma_initial_leading_e_creation'] = [r[1][0] for r in results]
        data_dict['electrons_stopped'] = [r[1][1] for r in results]
        data_dict['protons_inside'] = [r[1][2] for r in results]
        data_dict['electrons_inside'] = [r[1][3] for r in results]
    
    # Save to HDF5
    print("\n" + "=" * 78)
    save_to_h5(data_dict, output_h5, configIN)
    print(f"Saved to {output_h5}...")
    sys.stdout.flush() # CRITICAL: Forces immediate printing from the worker process
    
    return data_dict


def read_from_h5(h5_file: str, iteration: int = None) -> Dict:
    """
    Read data from HDF5 file.
    
    Args:
        h5_file: Path to HDF5 file
        iteration: Specific iteration to read (None = read all)
    
    Returns:
        Dictionary containing the data
    """
    with h5py.File(h5_file, 'r') as h5f:
        config = h5f.attrs['config']
        iterations = h5f.attrs['iterations']
        
        if iteration is not None:
            # Read specific iteration
            data = {'iteration': iteration, 'config': config}
            
            # Read based on configuration
            for group_name in h5f.keys():
                dataset_name = f'iteration_{iteration}'
                if dataset_name in h5f[group_name]:
                    data[group_name] = pd.DataFrame(h5f[group_name][dataset_name][()])
            
            return data
        else:
            # Read all iterations
            data = {
                'iterations': list(iterations),
                'config': config
            }
            
            # Read all groups based on configuration
            for group_name in h5f.keys():
                data[group_name] = []
                for iter_num in iterations:
                    dataset = h5f[group_name][f'iteration_{iter_num}'][()]
                    data[group_name].append(pd.DataFrame(dataset))
            
            return data


def list_h5_structure(h5_file: str):
    """
    Print the structure of an HDF5 file.
    
    Args:
        h5_file: Path to HDF5 file
    """
    with h5py.File(h5_file, 'r') as h5f:
        print(f"\n{'='*78}")
        print(f"HDF5 File: {h5_file}")
        print(f"{'='*78}")
        print(f"Configuration: {h5f.attrs['config']}")
        print(f"Number of iterations: {h5f.attrs['num_iterations']}")
        print(f"Iterations: {list(h5f.attrs['iterations'])}")
        print(f"\nGroups (datasets):")
        for group_name in h5f.keys():
            num_datasets = len(h5f[group_name].keys())
            print(f"  - {group_name}: {num_datasets} iterations")
        print(f"{'='*78}\n")


# Usage example
if __name__ == "__main__":
    # Parse command line arguments
    # Usage: python script.py <folder_path> [note] [config_tag] [max_iteration]
    
    if len(sys.argv) < 2:
        print("Usage: python script.py <folder_path> [note] [config_tag] [max_iteration]")
        print("\nExample:")
        print("  python script.py /path/to/root/files/ my_analysis onlyphotoemission 44")
        sys.exit(1)
    
    folder_path = sys.argv[1]
    output_h5 = sys.argv[2] if len(sys.argv) > 2 else 'single_config'
    configIN = sys.argv[3] if len(sys.argv) > 3 else 'onlyphotoemission'
    max_iteration = int(sys.argv[4]) if len(sys.argv) > 4 else 44
    
    # Use all available CPUs
    n_processes = os.cpu_count()
    
    # Construct directory path
    directory_path = os.path.join(folder_path, "root/")
    
    # Create output filename with metadata
    output_h5 = f"{output_h5}.h5"
    
    print(f"\n{'='*78}")
    print(f"ROOT to HDF5 Parallel Converter")
    print(f"{'='*78}")
    print(f"Configuration: {configIN}")
    print(f"Max iteration: {max_iteration}")
    print(f"Directory: {directory_path}")
    print(f"Output file: {output_h5}")
    print(f"Workers: {n_processes} (all CPUs)")
    print(f"{'='*78}\n")
    
    # Process files in parallel and save to HDF5
    data = parallel_root_to_h5(
        configIN=configIN,
        directory_path=directory_path,
        output_h5=output_h5,
        max_iteration=max_iteration,
        n_processes=n_processes
    )
    
    # View HDF5 structure
    list_h5_structure(output_h5)
    
    print(f"\nSuccessfully saved data to: {output_h5}")
    print(f"Total iterations processed: {len(data['iterations'])}")
    
    # Example: Read back all data
    # all_data = read_from_h5(output_h5)
    
    # Example: Read specific iteration
    # iteration_10 = read_from_h5(output_h5, iteration=10)
    # print(iteration_10['gamma_initial_leading_e_creation'].head())