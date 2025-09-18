import uproot
from scipy.constants import epsilon_0, e as q_e
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize
import matplotlib.colors as mcolors
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import glob

from mpl_toolkits.mplot3d.art3d import Poly3DCollection

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

    return convex_combined, face_illum

def plot_surface_potential_electrons_protons(electrons, protons, convex_combined, vmin=-0.01,vmax=0.01):

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

    return convex_combined, face_potentials

def plot_surface_potential_electrons_or_protons(electrons, convex_combined, vmin=-0.01,vmax=0.01, q = -1.602e-19 ):

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

    return convex_combined


def plot_electron_positions_slice(
    df,
    world_dimensions=(-0.05, 0.05),
    n_bins=100,
    iteration="Plot",
    stacked_spheres=None,
    plane="zx"  # "zx" or "zy"
):
    """
    plane: 'zx' plots Z vs X, 'zy' plots Z vs Y
    """
    # Define voxel edges
    x_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)
    y_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)
    z_edges = np.linspace(world_dimensions[0], world_dimensions[1], n_bins + 1)

    coords = np.vstack(df["Post_Step_Position_mm"])
    x, y, z = coords[:, 0], coords[:, 1], coords[:, 2]

    # Choose axes based on plane
    if plane.lower() == "zx":
        xdata, ydata = x, z
        ylabel, xlabel = "Z position (m)", "X position (m)"
    elif plane.lower() == "zy":
        xdata, ydata = y, z
        ylabel, xlabel = "Z position (m)", "Y position (m)"
    else:
        raise ValueError("plane must be 'zx' or 'zy'")

    # Create 2D histogram for density coloring
    hist, xedges, yedges = np.histogram2d(
        xdata, ydata,
        bins=n_bins,
        range=[[world_dimensions[0], world_dimensions[1]],
               [world_dimensions[0], world_dimensions[1]]]
    )

    # Plot 2D slice
    fig, ax = plt.subplots(figsize=(7, 6))
    sc = ax.scatter(xdata, ydata, s=2, c="red", alpha=0.7)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(f"{iteration} — {plane.upper()} slice")
    ax.set_aspect('equal', adjustable='box')

    plt.show()
    return fig, ax

def plot_positions_slice(
    df1,
    df2=None,
    iteration="Plot",
    stacked_spheres=None,
    plane="zx",  # "zx" or "zy"
    labels=["e-","protons"]
):
    """
    Plots a 2D slice of electron positions from one or two dataframes.
    
    Parameters:
        df1: First dataframe containing 'Post_Step_Position_mm' as list of 3D coordinates.
        df2: (Optional) Second dataframe to overlay.
        world_dimensions: Tuple specifying (min, max) bounds in meters.
        n_bins: Number of bins for histogram (if used).
        iteration: Title/label for plot.
        stacked_spheres: (Not used in this version).
        plane: 'zx' plots Z vs X, 'zy' plots Z vs Y.
    """
    
    # Convert from mm to meters
    coords1 = np.vstack(df1["Post_Step_Position_mm"]) * 1000
    x1, y1, z1 = coords1[:, 0], coords1[:, 1], coords1[:, 2]

    if df2 is not None:
        coords2 = np.vstack(df2["Post_Step_Position_mm"]) * 1000
        x2, y2, z2 = coords2[:, 0], coords2[:, 1], coords2[:, 2]
    else:
        x2 = y2 = z2 = None

    # Choose axes based on plane
    if plane.lower() == "zx":
        xdata1, ydata1 = x1, z1
        if df2 is not None:
            xdata2, ydata2 = x2, z2
        xlabel, ylabel = "X position (um)", "Z position (um)"
    elif plane.lower() == "zy":
        xdata1, ydata1 = y1, z1
        if df2 is not None:
            xdata2, ydata2 = y2, z2
        xlabel, ylabel = "Y position (um)", "Z position (um)"
    else:
        raise ValueError("plane must be 'zx' or 'zy'")

    # Plot
    fig, ax = plt.subplots(figsize=(7, 6))

    # Plot first dataset
    ax.scatter(xdata1, ydata1, s=2, c="red", alpha=0.6, label=labels[0])

    # Optionally overlay second dataset
    if df2 is not None:
        ax.scatter(xdata2, ydata2, s=2, c="blue", alpha=0.6, label=labels[1])

    
    if stacked_spheres is not None: 
        for contour in stacked_spheres.discrete:
            ax.plot(contour[:, 1], contour[:, 0], 'k-')  # black solid line

    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(f"{iteration} — {plane.upper()} slice")
    ax.set_aspect('equal', adjustable='box')
    ax.legend()

    
    plt.show()
    return fig, ax

def plot_potential_on_sphere(df, vmin=-0.07, vmax=0, iteration="iteration0"):

    coords_m = np.vstack(np.array(df["Post_Step_Position_mm"]))* 1e-3

    R = np.linalg.norm(coords_m, axis=1).max()

    n_theta = 200  # polar angle divisions
    n_phi = 400    # azimuthal divisions

    theta = np.linspace(0, np.pi, n_theta)
    phi = np.linspace(0, 2*np.pi, n_phi)
    theta_grid, phi_grid = np.meshgrid(theta, phi)

    x_s = R * np.sin(theta_grid) * np.cos(phi_grid)
    y_s = R * np.sin(theta_grid) * np.sin(phi_grid)
    z_s = R * np.cos(theta_grid)
    sphere_points = np.column_stack([x_s.ravel(), y_s.ravel(), z_s.ravel()])

    K = 1.0 / (4.0 * np.pi * epsilon_0 * 3.9)  # eps_r=3.9 for SiO2
    q = -q_e
    V_sphere = np.zeros(sphere_points.shape[0])

    for i, p in enumerate(sphere_points):
        r_vec = coords_m - p
        r = np.linalg.norm(r_vec, axis=1)
        V_sphere[i] = K * q * np.sum(1.0 / r)

    V_sphere_grid = V_sphere.reshape(theta_grid.shape)
    print(np.min(V_sphere_grid), np.max(V_sphere_grid))

    # Normalize colormap
    norm = mcolors.Normalize(vmin=vmin, vmax=vmax)
    facecolors = plt.cm.plasma(norm(V_sphere_grid))

    fig = plt.figure(figsize=(4, 6))
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(
        x_s*1e3, y_s*1e3, z_s*1e3,  # back to mm for display
        facecolors=facecolors,  
        rstride=1, cstride=1, antialiased=False, shade=False
    )
    mappable = plt.cm.ScalarMappable(cmap='plasma', norm=norm)
    mappable.set_array(V_sphere_grid)
    fig.colorbar(mappable, ax=ax, shrink=0.5, aspect=20, label='Potential (V)',orientation='horizontal',pad=-0.1)
    ax.set_aspect('equal')
    ax.set_title(iteration)
    ax.set_axis_off()
    plt.show()

    return fig, ax 

def calculate_stats(df):
    # --- Photoelectric Yield ---
    gamma_incident = df[
        (df["Particle_Type"] == "gamma") & (df["Parent_ID"] == 0.0)
    ].drop_duplicates(subset="Event_Number", keep="first")

    photelec_generated = df[
        (df["Particle_Type"] == "e-") & (df["Parent_ID"] == 1.0)
    ].drop_duplicates(subset="Event_Number", keep="last")

    photelec_holes = df[
        (df["Particle_Type"] == "e-") & (df["Parent_ID"] == 1.0)
    ].drop_duplicates(subset="Event_Number", keep="first")

    photelec_generated = photelec_generated[
        photelec_generated["Event_Number"].isin(gamma_incident["Event_Number"])
    ]

    photoelectric_yield = len(photelec_generated) / len(gamma_incident) if len(gamma_incident) > 0 else 0

    # --- Protons ---

    protons_incident = df[(df["Particle_Type"] == "proton") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    last_protons = df[(df["Particle_Type"] == "proton")].drop_duplicates(subset="Event_Number", keep="last")
    protons_inside = last_protons[(last_protons["Volume_Name_Post"] == "G4_SILICON_DIOXIDE")]

    protons_capture_fraction = len(protons_inside) / len(protons_incident) if len(protons_incident) > 0 else 0

    # --- Electrons ---

    electrons_incident = df[(df["Particle_Type"] == "e-") & (df["Parent_ID"] == 0.0)].drop_duplicates(subset="Event_Number", keep="first")

    last_electrons = df[(df["Particle_Type"] == "e-")].drop_duplicates(subset="Event_Number", keep="last")
    electrons_inside = last_electrons[(last_electrons["Volume_Name_Post"] == "G4_SILICON_DIOXIDE")]

    electrons_ejected = df[(df["Particle_Type"] == "e-") & \
                           (df["Volume_Name_Post"] == "World")].\
                            drop_duplicates(subset="Event_Number", keep="last")

    electrons_capture_fraction = len(electrons_inside) / len(electrons_incident) if len(electrons_incident) > 0 else 0

    # --- All electrons in SiO2, regardless of origin ---
    all_electrons_inside = df[
        (df["Volume_Name_Post"] == "G4_SILICON_DIOXIDE") &
        (df["Particle_Type"] == "e-")
    ].drop_duplicates(subset="Event_Number", keep="last")

    # --- Print outputs ---
    print(f"Photoelectric yield (e⁻ / γ): {photoelectric_yield:.4f} "
          f"({len(photelec_generated)} / {len(gamma_incident)})")
    if len(protons_incident) > 0:
        print(f"Protons captured in material: {protons_capture_fraction:.2%} "
            f"({len(protons_inside)} / {len(protons_incident)})")
    print(f"Electrons captured in material: {electrons_capture_fraction:.2%} "
          f"({len(electrons_inside)} / {len(electrons_incident)})")
    print(f"Electrons ejected in material: {len(electrons_ejected)}\n")

    return protons_inside, electrons_inside, photelec_holes, #all_electrons_inside, photelec_holes, electrons_ejected, 
