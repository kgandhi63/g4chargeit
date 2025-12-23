import uproot
from scipy.constants import epsilon_0, e as q_e
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize
import matplotlib.colors as mcolors
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import struct
from typing import Dict, Any
import glob
import os
import struct

import trimesh 
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

from numba import njit, prange
import numpy as np
import glob
import h5py, re

def load_h5_to_dict(filename):
    data_dict = {}

    with h5py.File(filename, "r") as f:
        # Sort keys numerically by the integer after "iter_"
        sorted_keys = sorted(f.keys(), key=lambda x: int(re.findall(r'\d+', x)[0]))

        for iter_key in sorted_keys:
            grp = f[iter_key]
            data_dict[iter_key] = {
                "pos": grp["pos"][:],
                "E": grp["E"][:],
                "E_mag": grp["E_mag"][:],
                "gradRefinements": grp.attrs["gradRefinements"]
            }

    return data_dict

def read_rootfile(file, directory_path=None, columns=None):
    """
    Read a ROOT file and return a pandas DataFrame with only the requested columns.
    
    Parameters:
        file : str
            ROOT filename.
        directory_path : str
            Path to the directory containing the file.
        columns : list of str, optional
            List of columns (branches) to read. If None, reads all branches.
    
    Returns:
        pd.DataFrame
    """
    file_path = f"{directory_path}/{file}" if directory_path else file
    tree_name = "Hit Data"

    with uproot.open(file_path) as root_file:
        tree = root_file[tree_name]

        # If columns not specified, read all branches
        if columns is None:
            columns = tree.keys()

        branch_vars = {}
        for name in columns:
            if name in tree.keys():
                branch_vars[name] = tree[name].array(library="np")
            else:
                print(f"Warning: branch '{name}' not found in tree '{tree_name}'")

    df = pd.DataFrame(branch_vars)

    # Optionally compute Kinetic_Energy_Diff_eV only if pre/post columns are present
    if 'Kinetic_Energy_Pre_MeV' in df.columns and 'Kinetic_Energy_Post_MeV' in df.columns:
        df['Kinetic_Energy_Diff_eV'] = (df['Kinetic_Energy_Pre_MeV'] - df['Kinetic_Energy_Post_MeV']) * 1e6

    return df

def read_uniform_fieldmap(filename: str) -> pd.DataFrame:
    """
    Read C++ binary field map into pandas DataFrame
    """
    with open(filename, 'rb') as f:
        # Read header
        dimensions = np.frombuffer(f.read(3 * 4), dtype=np.int32)  # 3 ints
        min_coords = np.frombuffer(f.read(3 * 8), dtype=np.float64)  # 3 doubles
        step_size = np.frombuffer(f.read(3 * 8), dtype=np.float64)  # 3 doubles
        storage_type = np.frombuffer(f.read(4), dtype=np.int32)[0]  # 1 int
        
        nx, ny, nz = dimensions
        total_points = nx * ny * nz
        
        # print(f"Reading field map: {nx} x {ny} x {nz} = {total_points:,} points")
        # print(f"Min coordinates: {min_coords}")
        # print(f"Step size: {step_size}")
        # print(f"Storage type: {'Double' if storage_type == 0 else 'Float'}")
        
        # Read field data
        if storage_type == 0:  # Double precision
            field_data = np.frombuffer(f.read(total_points * 3 * 8), dtype=np.float64)
        else:  # Float precision  
            field_data = np.frombuffer(f.read(total_points * 3 * 4), dtype=np.float32)
        
        # Reshape to 4D: [x, y, z, components]
        field_data = field_data.reshape(nx, ny, nz, 3)
        
        # Create coordinate arrays
        x_coords = min_coords[0] + np.arange(nx) * step_size[0]
        y_coords = min_coords[1] + np.arange(ny) * step_size[1]
        z_coords = min_coords[2] + np.arange(nz) * step_size[2]
        
        # Create meshgrid and flatten for DataFrame
        X, Y, Z = np.meshgrid(x_coords, y_coords, z_coords, indexing='ij')
        
        # Create DataFrame
        df = pd.DataFrame({
            'x': X.flatten(),
            'y': Y.flatten(), 
            'z': Z.flatten(),
            'Ex': field_data[:, :, :, 0].flatten()*1e9, # convert field units from 1 MeV / e / mm = 1e6 V / mm = 1e9 V/m
            'Ey': field_data[:, :, :, 1].flatten()*1e9,
            'Ez': field_data[:, :, :, 2].flatten()*1e9
        })
        
        # Add magnitude column
        df['E_mag'] = np.sqrt(df['Ex']**2 + df['Ey']**2 + df['Ez']**2)
        
        return df

# Numba functions are fine, but ensure they are imported/defined.
@njit(fastmath=True, cache=True)
def scale_fields_numba(field_data):
    """
    Scale Ex, Ey, Ez components (columns 3 to 5) by 1e9 in place.
    """
    # Use array slicing for Numba compatibility and efficiency
    field_data *= 1e9
    return field_data

# Fast norm function with Numba
@njit(parallel=True)
def compute_E_mag(Ex, Ey, Ez):
    n = Ex.shape[0]
    result = np.empty(n, dtype=Ex.dtype) # Use the input dtype
    for i in prange(n):
        result[i] = np.sqrt(Ex[i]**2 + Ey[i]**2 + Ez[i]**2)
    return result

# --- Optimized Reader Function ---

def read_adaptive_fieldmap(filename):
    """
    Reads the adaptive field map binary file using a highly optimized
    NumPy approach to process the node data in a single step.
    """
    
    # Define C++ data types for Python
    C_DOUBLE = 8 # 8 bytes
    C_UINT32 = 4 # 4 bytes
    C_UINT64 = 8 # 8 bytes

    # Define the *Dtype* for a single node block (7 floats + 1 byte)
    # The 'b' field is for the uint8_t (is_leaf) flag.
    # The 'padding' is necessary because C++ structs often align to 4 or 8 bytes.
    # A single node is 25 bytes. We must assume NO PADDING in the C++ file writing,
    # or that the C++ code wrote the fields sequentially without internal padding.
    # Assuming sequential write for this optimization:
    NODE_DTYPE = np.dtype([
        ('pos', '<f4', 3),     # 3*float32 (center_x, y, z)
        ('field', '<f4', 3),   # 3*float32 (E_x, y, z)
    ])
    
    with open(filename, 'rb') as f:
        
        # Read Octree Parameters

        # max_d (uint32_t)
        max_d = np.fromfile(f, dtype=np.uint32, count=1)[0]
        
        # min_s (double/float64) - Assuming G4double is 8 bytes
        min_s = np.fromfile(f, dtype=np.float64, count=1)[0]
        
        # total_node_count (uint64_t)
        total_node_count = np.fromfile(f, dtype=np.uint64, count=1)[0]
        
        # final_leaf_count (uint64_t)
        final_leaf_count = np.fromfile(f, dtype=np.uint64, count=1)[0] 
        
        # --- 2. Read ALL Field Nodes Data ---
            
        # Read the *entire* block of (total_node_count * 24 bytes) in one go
        node_raw_array = np.fromfile(f, dtype=NODE_DTYPE, count=total_node_count)

        # Separate the structured data into a flat (N, 6) array
        # This creates the final expected data array: (pos_x, y, z, E_x, y, z)
        field_data = np.hstack([
            node_raw_array['pos'],
            node_raw_array['field'],
        ]).astype(np.float32) # Ensure the final array is float32

        # --- Construct Metadata and Output ---
        metadata = {
            'mesh_parameters': {
                'max_depth': max_d,
                'min_step_internal': min_s, 
                'total_nodes': total_node_count,
                'final_leaf_nodes': final_leaf_count
            }
        }
        
        return field_data, metadata

# Efficient, memory-aware loader
def read_data_format_efficient(filenames, scaling=True):
    fields_all = {}

    for fileIN in filenames:
        iteration = int(fileIN.split("/")[-1].split("-")[0])

        # Minimal fieldmap reader - replace with your efficient implementation
        data, metadata = read_adaptive_fieldmap(fileIN)  # assume this returns a pandas dataframe

        # Slice columns directly from NumPy array
        pos = data[:, :3].astype(np.float32, copy=False)    # x, y, z
        if scaling:
            Ex  = scale_fields_numba(data[:, 3].astype(np.float32, copy=False))
            Ey  = scale_fields_numba(data[:, 4].astype(np.float32, copy=False))
            Ez  = scale_fields_numba(data[:, 5].astype(np.float32, copy=False))
        else:
            Ex  = data[:, 3].astype(np.float32, copy=False)
            Ey  = data[:, 4].astype(np.float32, copy=False)
            Ez  = data[:, 5].astype(np.float32, copy=False)
            
        # Compute E-field magnitude
        E_mag = compute_E_mag(Ex, Ey, Ez)

        # Store in dictionary
        fields_all[iteration] = {
            "pos": pos,
            "E": np.stack((Ex, Ey, Ez), axis=1),  # shape (N, 3)
            "E_mag": E_mag, 
            "gradRefinements": int(metadata['mesh_parameters']['total_nodes'] - metadata['mesh_parameters']['final_leaf_nodes'])
        }

    return fields_all

def compute_nearest_field_vector(fields_dict: Dict[int, Dict[str, np.ndarray]], target: np.ndarray = None, start: int = 1) -> Dict[str, np.ndarray]:
    """
    For each field iteration, find the electric field vector (Ex, Ey, Ez)
    and magnitude at the grid point closest to the specified target location.
    
    This optimized version uses pre-allocated NumPy arrays to avoid the overhead 
    of list appending and final array conversion.

    Parameters:
        fields_dict (dict): Dictionary mapping iteration number to field data. 
                            Each field must have keys 'pos' (Nx3), 'E' (Nx3), and 'E_mag' (N,).
        target (array-like): 3D point to compare distances to. If None, defaults to [-0.1, 0, 0.122].
        start (int): First iteration number to consider.

    Returns:
        Dict[str, np.ndarray]: Dictionary with keys 'iter', 'E', and 'E_mag', containing 
                               pre-allocated NumPy arrays.
    """

    if target is None:
        # Default based on the original problem's common target
        target = np.array([-0.1, 0, 0.122])  
    else:
        target = np.asarray(target)

    # Filter keys and ensure they are sorted
    iterations = sorted([k for k in fields_dict.keys() if k >= start]) 
    N = len(iterations) # Determine the required size for pre-allocation

    # --- Pre-allocate arrays for optimized assignment ---
    iter_array = np.empty(N, dtype=int)
    E_array = np.empty((N, 3), dtype=np.float64) # N rows, 3 columns (Ex, Ey, Ez)
    E_mag_array = np.empty(N, dtype=np.float64)

    for i, iteration in enumerate(iterations):
        field = fields_dict[iteration]
        positions = field["pos"]
        efield = field["E"]
        emag = field["E_mag"]

        # Vectorized distance computation (distance between positions and target)
        distances = np.linalg.norm(positions - target, axis=1)

        # Index of closest point
        idx = np.argmin(distances)

        # Store results directly into the pre-allocated array indices
        iter_array[i] = iteration
        E_array[i] = efield[idx]
        E_mag_array[i] = emag[idx]

    # Return the pre-allocated arrays
    return {
        "iter": iter_array, 
        "E": E_array, 
        "E_mag": E_mag_array
    }


def plot_electron_positions(df,world_dimensions = (-0.05, 0.05), n_bins=100, iteration="iteration0", stacked_spheres=None):

    # Define voxel edges
    x_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)
    y_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)
    z_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)

    coords = np.vstack(df["Post_Step_Position_mm"])  # mm to m
    x, y, z = coords[:, 0], coords[:, 1], coords[:, 2]

    # Compute 3D histogram: sum of energy lost in each voxel
    hist_electrons, edges = np.histogramdd(
        np.column_stack((x, y, z)),
        bins=(x_edges, y_edges, z_edges)
    )

    # Get voxel centers for plotting
    x_centers = 0.5 * (x_edges[:-1] + x_edges[1:])
    y_centers = 0.5 * (y_edges[:-1] + y_edges[1:])
    z_centers = 0.5 * (z_edges[:-1] + z_edges[1:])
    X, Y, Z = np.meshgrid(x_centers, y_centers, z_centers, indexing='ij')

    # Mask nonzero voxels and flatten arrays for plotting
    mask = hist_electrons > 0
    Xf_electrons = X[mask]
    Yf_electrons = Y[mask]
    Zf_electrons = Z[mask]
    hist_values_electrons = hist_electrons[mask]

    # Plot
    fig = plt.figure(figsize=(8, 6))
    ax = fig.add_subplot(111, projection="3d")


    if stacked_spheres is not None:

        # Create a collection of polygons (triangles)
        collection = Poly3DCollection(stacked_spheres.vectors, alpha=0.2, edgecolor="darkblue",linewidth=0.1)
        collection.set_facecolor('lightblue')
        ax.add_collection3d(collection)

        # Auto scale to the mesh size
        scale = stacked_spheres.points.flatten()
        ax.auto_scale_xyz(scale, scale, scale)
    else:

        # Set sphere parameters
        radius = 0.05  # Adjust radius as needed
        # Create spherical coordinates
        u = np.linspace(0, 2 * np.pi, 100)
        v = np.linspace(0, np.pi, 100)
        sphere_x = radius * np.outer(np.cos(u), np.sin(v))
        sphere_y = radius * np.outer(np.sin(u), np.sin(v))
        sphere_z = radius * np.outer(np.ones_like(u), np.cos(v))
        ax.plot_surface(sphere_x, sphere_y, sphere_z, color='red', alpha=0.3, linewidth=0)


    #sc = ax.scatter(Xf_electrons, Yf_electrons, Zf_electrons, s=5, c=hist_values_electrons, cmap='Reds', norm=LogNorm(vmin=1, vmax=5), alpha=0.8)
    sc = ax.scatter(x,y,z, s=2, norm=LogNorm(vmin=1, vmax=5), alpha=0.8,color="red")
    #fig.colorbar(sc, ax=ax, label='')

    ax.set_title(iteration)
    #ax.set_aspect('equal')
    plt.show()

    return fig, ax 

def plot_trimesh_edges_only(mesh, edge_color=[0, 0, 0, 128]):  # Black with 50% transparency
    """Plot only the edges with specified color and transparency"""
    
    # Create edge visualizations
    edges = mesh.edges_unique
    vertices = mesh.vertices
    
    # Create a collection of lines for each edge
    lines = []
    for edge in edges:
        lines.append(trimesh.path.entities.Line(edge))
    
    # Create colors for each entity (each line) with RGBA
    # Ensure we have 4 values (RGBA) for each color
    if len(edge_color) == 3:
        edge_color = list(edge_color) + [128]  # Add alpha if only RGB provided
    
    colors = np.tile(edge_color, (len(lines), 1))
    
    # Create a Path3D object
    path = trimesh.path.Path3D(
        entities=lines,
        vertices=vertices,
        colors=colors
    )
    
    scene = trimesh.Scene()
    scene.add_geometry(path)
    return scene

def plot_face_illumination(incident_particles, convex_combined, vmin=0, vmax=1):
    """
    Colors the mesh faces based on how many particles are nearest to each face.
    """
    # Step 1: Get particle positions as Nx3 array
    points = np.array(incident_particles["Pre_Step_Position_mm"].tolist())

    # Step 2: Get nearest face indices
    _, _, face_ids = convex_combined.nearest.on_surface(points)

    # Step 3: Accumulate per-face "illumination"
    face_illum = np.zeros(len(convex_combined.faces))
    np.add.at(face_illum, face_ids, 1)

    # Step 4: Normalize face illumination (optional)
    norm_f = face_illum.astype(float)

    # Step 5: Apply colormap
    cmap = plt.cm.OrRd
    norm_func = Normalize(vmin=vmin, vmax=vmax)
    rgb_f = cmap(norm_func(norm_f))[:, :3]

    # Step 6: Build RGBA face colors
    colors_f = np.zeros((len(face_illum), 4), dtype=np.uint8)
    colors_f[:, :3] = (rgb_f * 255).astype(np.uint8)
    colors_f[:, 3] = 255  # Fully opaque

    # Step 7: Apply colors to faces
    convex_combined.visual.face_colors = colors_f

    return convex_combined, face_illum, colors_f

def calculate_stats(df, config="photoemission", volume_name="SiO2",printout=False):

    # --- Photoemission Case ---

    incident_gamma = df[(df["Particle_Type"] == "gamma")  & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    # get dataframe of the last e- event within the sensitive detector
    last_e_event = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] > 0.0)].drop_duplicates(subset="Event_Number", keep="last")
    # get new dataframe of all e- that left the world
    world_e_energy=last_e_event[(last_e_event["Volume_Name_Post"]=="physical_cyclic") | (last_e_event["Volume_Name_Pre"]=="physical_cyclic")]

    # get all the electrons stopped in the material 
    electrons_stopped = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] > 0.0) & (df["Kinetic_Energy_Post_MeV"] ==0.0) & (df["Volume_Name_Post"]==volume_name)]

    # get all the initial gamma energy that leads to an e- escaping
    matching_event_numbers = np.intersect1d(df["Event_Number"], world_e_energy["Event_Number"])
    gamma_initial_leading_to_e_ejection = df[df["Event_Number"].isin(matching_event_numbers)].drop_duplicates(subset="Event_Number", keep="first")

    #  get all the initial gamma energy that leds to the creation of e-
    matching_event_numbers = np.intersect1d(df["Event_Number"], last_e_event["Event_Number"])
    gamma_initial_leading_e_creation = df[df["Event_Number"].isin(matching_event_numbers)].drop_duplicates(subset="Event_Number", keep="first")

    photoelectric_yield = len(gamma_initial_leading_e_creation) / len(incident_gamma) if len(incident_gamma) > 0 else 0

    # --- Solar Wind Case ---

    protons_incident = df[(df["Particle_Type"] == "proton") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    last_protons = df[(df["Particle_Type"] == "proton")].drop_duplicates(subset="Event_Number", keep="last")
    protons_inside = last_protons[(last_protons["Volume_Name_Post"] == volume_name)]

    protons_capture_fraction = len(protons_inside) / len(protons_incident) if len(protons_incident) > 0 else 0

    electrons_incident = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    last_electrons = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="last")
    electrons_inside = last_electrons[(last_electrons["Volume_Name_Post"] == volume_name)]

    electrons_capture_fraction = len(electrons_inside) / len(electrons_incident) if len(electrons_incident) > 0 else 0

    # --- Print outputs ---
    if "photoemission" in config:

        if printout == True:
            print(f"Photoelectric yield (holes/ γ): {photoelectric_yield:.4f} "
                f"({len(gamma_initial_leading_e_creation)} / {len(incident_gamma)})")
            print(f"e- stopped in material: {len(electrons_stopped)}")
            print(f"Electrons ejected, from photoelectric effect: {len(gamma_initial_leading_to_e_ejection)}\n")

        return gamma_initial_leading_e_creation, electrons_stopped, gamma_initial_leading_to_e_ejection
    
    elif "solarwind" in config:

        if printout == True:
            print(f"Incident H+ stopped in material: {protons_capture_fraction:.2%} "
                f"({len(protons_inside)} / {len(protons_incident)})")
            print(f"Incident e- stopped in material: {electrons_capture_fraction:.2%} "
                f"({len(electrons_inside)} / {len(electrons_incident)})\n")

        return protons_inside, electrons_inside 

    elif "allparticles" in config:

        if printout == True:
            print(f"Photoelectric yield (holes/ γ): {photoelectric_yield:.4f} "
                f"({len(gamma_initial_leading_e_creation)} / {len(incident_gamma)})")
            print(f"e- stopped in material: {len(electrons_stopped)}")
            print(f"Electrons ejected, from photoelectric effect: {len(gamma_initial_leading_to_e_ejection)}")
            print(f"Incident H+ stopped in material: {protons_capture_fraction:.2%} "
                f"({len(protons_inside)} / {len(protons_incident)})")
            print(f"Incident e- stopped in material: {electrons_capture_fraction:.2%} "
                f"({len(electrons_inside)} / {len(electrons_incident)})\n")

        return gamma_initial_leading_e_creation, electrons_stopped, protons_inside, electrons_inside 
