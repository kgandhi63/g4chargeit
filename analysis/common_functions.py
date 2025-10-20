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

def read_rootfile(file,directory_path=None):
    #file = '00_iteration0_num5000.root' #'13_iteration5_from_num1000000.root'
    file_path = f"{directory_path}/{file}"
    tree_name = "Hit Data"

    with uproot.open(file_path) as file:
        
        tree = file[tree_name]
        branch_names = tree.keys()
        branch_vars = {}
        for name in branch_names:
            branch_vars[name] = tree[name].array(library="np")

    df = pd.DataFrame(branch_vars)
    df['Kinetic_Energy_Diff_eV'] = (df['Kinetic_Energy_Pre_MeV'] - df['Kinetic_Energy_Post_MeV'])*1e6 # convert to eV

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

@njit(fastmath=True, cache=True)
def scale_fields_numba(field_data):
    """
    Scale Ex, Ey, Ez components (columns 3 to 5) by 1e9 in place.
    """
    for i in range(field_data.shape[0]):
        for j in range(3, 6):
            field_data[i, j] *= 1e9
    return field_data

# Fast norm function with Numba
@njit(parallel=True)
def compute_E_mag(Ex, Ey, Ez):
    n = Ex.shape[0]
    result = np.empty(n)
    for i in prange(n):
        result[i] = np.sqrt(Ex[i]**2 + Ey[i]**2 + Ez[i]**2)
    return result

def read_adaptive_fieldmap(filename, scaling=True):
    with open(filename, 'rb') as f:
        # --- Read header ---
        world_bounds = np.frombuffer(f.read(6 * 8), dtype=np.float64)
        mesh_parameters = np.frombuffer(f.read(5 * 4), dtype=np.uint32)

        max_depth, min_step_um, total_nodes, leaf_nodes, storage_flag = mesh_parameters

        # --- File layout ---
        file_size = os.path.getsize(filename)
        header_bytes = 6 * 8 + 5 * 4
        statistics_bytes = 3 * 4
        remaining_bytes = file_size - header_bytes

        node_size_bytes = 6 * 8  # 6 doubles = 48 bytes
        field_data_bytes = remaining_bytes - statistics_bytes

        if field_data_bytes % node_size_bytes != 0:
            raise ValueError(f"Field data size not divisible by node size (48 bytes).")

        num_nodes = field_data_bytes // node_size_bytes

        # --- Read and reshape field data ---
        field_data = np.frombuffer(f.read(field_data_bytes), dtype=np.float64)
        field_data = field_data.reshape((num_nodes, 6)).copy()  
        
        if scaling:
            field_data = scale_fields_numba(field_data)

        # --- Read statistics ---
        statistics = np.frombuffer(f.read(statistics_bytes), dtype=np.int32)
        gradient_refinements, max_depth_reached, num_positions = statistics

    metadata = {
        'world_bounds': world_bounds,
        'mesh_parameters': {
            'max_depth': max_depth,
            'min_step_um': min_step_um,
            'total_nodes': total_nodes,
            'leaf_nodes': leaf_nodes,
            'storage_flag': storage_flag
        },
        'statistics': {
            'gradient_refinements': gradient_refinements,
            'max_depth_reached': max_depth_reached,
            'fPositions_size': num_positions,
        }
    }

    return field_data, metadata

# Efficient, memory-aware loader
def read_data_format_efficient(filenames, scaling=True):
    fields_all = {}

    for fileIN in filenames:
        iteration = int(fileIN.split("/")[-1].split("-")[0])

        # Minimal fieldmap reader - replace with your efficient implementation
        data, _ = read_adaptive_fieldmap(fileIN,scaling=scaling)  # assume this returns a pandas dataframe

        # Slice columns directly from NumPy array
        pos = data[:, :3].astype(np.float32, copy=False)    # x, y, z
        Ex  = data[:, 3].astype(np.float32, copy=False)
        Ey  = data[:, 4].astype(np.float32, copy=False)
        Ez  = data[:, 5].astype(np.float32, copy=False)

        # Compute E-field magnitude
        E_mag = compute_E_mag(Ex, Ey, Ez)

        # Store in dictionary
        fields_all[iteration] = {
            "pos": pos,
            "E": np.stack((Ex, Ey, Ez), axis=1),  # shape (N, 3)
            "E_mag": E_mag
        }

    return fields_all

def compute_nearest_field_vector(fields_SW, target=None, start=1, end=None):
    """
    For each field iteration, find the electric field vector (Ex, Ey, Ez)
    at the grid point closest to the specified target location.

    Parameters:
        fields_SW (dict): Dictionary mapping iteration number to field data. 
                          Each field must have keys 'pos' (Nx3) and 'E' (Nx3).
        target (array-like): 3D point to compare distances to. If None, defaults to [-0.1, 0, 0.122].
        start (int): First iteration number to consider.
        end (int or None): Last iteration number to consider. If None, uses max key in fields_SW.
        compute_magnitude (bool): Whether to return E-field magnitudes.

    Returns:
        pd.DataFrame: Columns = ["iter", "Ex", "Ey", "Ez"] (+ optional "|E|")
    """

    if target is None:
        target = np.array([-0.1, 0, 0.122])  # Default based on original code
    else:
        target = np.asarray(target)

    if end is None:
        end = max(fields_SW)

    # Pre-allocate results
    result_array = np.empty((len(fields_SW.keys()), 5), dtype=np.float64)

    for i, iteration in enumerate(fields_SW.keys()):
        field = fields_SW[iteration]
        positions = field["pos"]
        efield = field["E"]
        emag = field["E_mag"]

        # Vectorized distance computation
        distances = np.linalg.norm(positions - target, axis=1)

        # Index of closest point
        idx = np.argmin(distances)

        # Extract E-field components
        result_array[i, 0] = iteration
        result_array[i, 1:-1] = efield[idx] # 3 components
        result_array[i, 4] = emag[idx]

    # Create DataFrame
    df = pd.DataFrame(result_array, columns=["iter", "Ex", "Ey", "Ez", "E_mag"])

    return df


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

def plot_surface_potential_fornegativepositive_charge(electrons, protons, convex_combined, vmin=-0.01,vmax=0.01):

    point = np.array(electrons["Pre_Step_Position_mm"].tolist())  # Your point coordinate
    _, distance_e, face_id_e = convex_combined.nearest.on_surface(point)

    # Compute potential at each surface point due to each electron
    q = -1.602e-19  # electron charge in Coulombs
    potentials_e = 1 / (4 * np.pi * epsilon_0) * q / (abs(distance_e - 0.1)*0.001 + 1e-12) * 1000 # result is an array of potentials, in mV
    # for debugging: np.array([-1]*len(distance_e)) 
    
    point = np.array(protons["Pre_Step_Position_mm"].tolist())  # Your point coordinate
    _, distance_p, face_id_p = convex_combined.nearest.on_surface(point)

    # Compute potential at each surface point due to each electron
    q = +1.602e-19  # electron charge in Coulombs
    potentials_p =1 / (4 * np.pi * epsilon_0) * q / (abs(distance_p - 0.1)*0.001 + 1e-12) * 1000 # result is an array of potentials, in mV
    # for debugging: np.array([1]*len(distance_p))#

    # Find unique face IDs and inverse indices for electrons
    unique_faces_e, inverse_indices_e = np.unique(face_id_e, return_inverse=True)

    # Initialize array to sum potentials for each unique face (electrons)
    potentials_sum_e = np.zeros(len(unique_faces_e))

    # Accumulate potentials for each face using inverse indices
    np.add.at(potentials_sum_e, inverse_indices_e, potentials_e)

    # --- Protons ---

    # Find unique face IDs and inverse indices for protons
    unique_faces_p, inverse_indices_p = np.unique(face_id_p, return_inverse=True)

    # Initialize array to sum potentials for each unique face (protons)
    potentials_sum_p = np.zeros(len(unique_faces_p))

    # Accumulate potentials for each face using inverse indices
    np.add.at(potentials_sum_p, inverse_indices_p, potentials_p)

    # --- Combine electron and proton potentials per face ---

    # Initialize full array for face potentials (length = total number of faces)
    face_potentials = np.zeros(len(convex_combined.faces))

    # Add electron potentials to their respective faces
    face_potentials[unique_faces_e] += potentials_sum_e

    # Add proton potentials to their respective faces (accumulate)
    face_potentials[unique_faces_p] += potentials_sum_p

    # Prepare RGBA array initialized to transparent
    colors_rgba = np.zeros((len(face_potentials), 4), dtype=np.uint8)
    norm = np.zeros_like(face_potentials)
    norm = face_potentials

    # Step 5: Apply colormap
    cmap = plt.cm.seismic
    norm_func = Normalize(vmin=vmin, vmax=vmax)
    colors_rgb = cmap(norm_func(norm))[:, :3]

    # Assign RGB to all faces
    colors_rgba[:, :3] = (colors_rgb * 255).astype(np.uint8)

    # Apply colors to mesh
    convex_combined.visual.face_colors = colors_rgba

    return convex_combined, face_potentials, colors_rgba

def plot_surface_potential_allparticle_case(gammas, photoelectrons, protons, electrons, convex_combined, vmin=-0.01, vmax=0.01):
    # --- Combine negative charges: electrons + photoelectrons ---
    neg_particles = pd.concat([electrons, photoelectrons], ignore_index=True)
    neg_points = np.array(neg_particles["Pre_Step_Position_mm"].tolist())
    _, distance_neg, face_id_neg = convex_combined.nearest.on_surface(neg_points)

    # Compute potential for negative charges
    q_neg = -1.602e-19  # Coulombs
    potentials_neg = 1 / (4 * np.pi * epsilon_0) * q_neg / (np.abs(distance_neg - 0.1) * 0.001 + 1e-12) * 1000  # mV

    # --- Combine positive charges: protons + gammas ---
    pos_particles = pd.concat([protons, gammas], ignore_index=True)
    pos_points = np.array(pos_particles["Pre_Step_Position_mm"].tolist())
    _, distance_pos, face_id_pos = convex_combined.nearest.on_surface(pos_points)

    # Compute potential for positive charges
    q_pos = +1.602e-19  # Coulombs
    potentials_pos = 1 / (4 * np.pi * epsilon_0) * q_pos / (np.abs(distance_pos - 0.1) * 0.001 + 1e-12) * 1000  # mV

    # --- Aggregate potentials per face (negative) ---
    unique_faces_neg, inverse_indices_neg = np.unique(face_id_neg, return_inverse=True)
    potentials_sum_neg = np.zeros(len(unique_faces_neg))
    np.add.at(potentials_sum_neg, inverse_indices_neg, potentials_neg)

    # --- Aggregate potentials per face (positive) ---
    unique_faces_pos, inverse_indices_pos = np.unique(face_id_pos, return_inverse=True)
    potentials_sum_pos = np.zeros(len(unique_faces_pos))
    np.add.at(potentials_sum_pos, inverse_indices_pos, potentials_pos)

    # --- Combine into face potentials array ---
    face_potentials = np.zeros(len(convex_combined.faces))
    face_potentials[unique_faces_neg] += potentials_sum_neg
    face_potentials[unique_faces_pos] += potentials_sum_pos

    # --- Visualization ---
    colors_rgba = np.zeros((len(face_potentials), 4), dtype=np.uint8)
    cmap = plt.cm.seismic
    norm_func = Normalize(vmin=vmin, vmax=vmax)
    colors_rgb = cmap(norm_func(face_potentials))[:, :3]
    colors_rgba[:, :3] = (colors_rgb * 255).astype(np.uint8)

    convex_combined.visual.face_colors = colors_rgba

    return convex_combined, face_potentials


def plot_surface_potential_one_particle_type(electrons, convex_combined, vmin=-0.01,vmax=0.01, q = -1.602e-19 ):

    point = np.array(electrons["Pre_Step_Position_mm"].tolist())  # Your point coordinate
    _, distance_e, face_id_e = convex_combined.nearest.on_surface(point)

    # Compute potential at each surface point due to each electron
    potentials_e = 1 / (4 * np.pi * epsilon_0) * q / (abs(distance_e - 0.1)*0.001 + 1e-12) * 1000 # result is an array of potentials, in mV
    # for debugging: np.array([-1]*len(distance_e)) 

    # Find unique face IDs and inverse indices for electrons
    unique_faces_e, inverse_indices_e = np.unique(face_id_e, return_inverse=True)

    # Initialize array to sum potentials for each unique face (electrons)
    potentials_sum_e = np.zeros(len(unique_faces_e))

    # Accumulate potentials for each face using inverse indices
    np.add.at(potentials_sum_e, inverse_indices_e, potentials_e)

    # --- Combine electron and proton potentials per face ---

    # Initialize full array for face potentials (length = total number of faces)
    face_potentials = np.zeros(len(convex_combined.faces))

    # Add electron potentials to their respective faces
    face_potentials[unique_faces_e] += potentials_sum_e

    # Prepare RGBA array initialized to transparent
    colors_rgba = np.zeros((len(face_potentials), 4), dtype=np.uint8)

    # Normalize only non-zero potentials
    norm = np.zeros_like(face_potentials)
    norm = (face_potentials - vmin) / (vmax - vmin)
    norm = np.clip(norm, 0, 1)

    # Map normalized potentials to RGB
    cmap = plt.cm.seismic
    colors_rgb = cmap(norm)[:, :3]

    # Assign RGB to all faces
    colors_rgba[:, :3] = (colors_rgb * 255).astype(np.uint8)

    # Apply colors to mesh
    convex_combined.visual.face_colors = colors_rgba

    return convex_combined, face_potentials

def calculate_stats(df, config="photoemission"):

    # --- Photoemission Case ---

    incident_gamma = df[(df["Particle_Type"] == "gamma")  & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    # get dataframe of the last e- event within the sensitive detector
    last_e_event = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] > 0.0)].drop_duplicates(subset="Event_Number", keep="last")
    # get new dataframe of all e- that left the world
    world_e_energy=last_e_event[(last_e_event["Volume_Name_Post"]=="physical_cyclic") | (last_e_event["Volume_Name_Pre"]=="physical_cyclic")]

    # get all the electrons stopped in the material 
    electrons_stopped = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] > 0.0) & (df["Kinetic_Energy_Post_MeV"] ==0.0) & (df["Volume_Name_Post"]=="G4_SILICON_DIOXIDE")]

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
    protons_inside = last_protons[(last_protons["Volume_Name_Post"] == "G4_SILICON_DIOXIDE")]

    protons_capture_fraction = len(protons_inside) / len(protons_incident) if len(protons_incident) > 0 else 0

    electrons_incident = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    last_electrons = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="last")
    electrons_inside = last_electrons[(last_electrons["Volume_Name_Post"] == "G4_SILICON_DIOXIDE")]

    electrons_capture_fraction = len(electrons_inside) / len(electrons_incident) if len(electrons_incident) > 0 else 0

    # --- Print outputs ---
    if "photoemission" in config:

        print(f"Photoelectric yield (holes/ γ): {photoelectric_yield:.4f} "
            f"({len(gamma_initial_leading_e_creation)} / {len(incident_gamma)})")
        print(f"e- stopped in material: {len(electrons_stopped)}")
        print(f"Electrons ejected, from photoelectric effect: {len(gamma_initial_leading_to_e_ejection)}\n")

        return gamma_initial_leading_e_creation, electrons_stopped, gamma_initial_leading_to_e_ejection
    
    elif "solarwind" in config:
        print(f"Incident H+ stopped in material: {protons_capture_fraction:.2%} "
            f"({len(protons_inside)} / {len(protons_incident)})")
        print(f"Incident e- stopped in material: {electrons_capture_fraction:.2%} "
            f"({len(electrons_inside)} / {len(electrons_incident)})\n")

        return protons_inside, electrons_inside 

    elif "allparticles" in config:

        print(f"Photoelectric yield (holes/ γ): {photoelectric_yield:.4f} "
            f"({len(gamma_initial_leading_e_creation)} / {len(incident_gamma)})")
        print(f"e- stopped in material: {len(electrons_stopped)}")
        print(f"Electrons ejected, from photoelectric effect: {len(gamma_initial_leading_to_e_ejection)}")
        print(f"Incident H+ stopped in material: {protons_capture_fraction:.2%} "
            f"({len(protons_inside)} / {len(protons_incident)})")
        print(f"Incident e- stopped in material: {electrons_capture_fraction:.2%} "
            f"({len(electrons_inside)} / {len(electrons_incident)})\n")

        return gamma_initial_leading_e_creation, electrons_stopped, protons_inside, electrons_inside 
