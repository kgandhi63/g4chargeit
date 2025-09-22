import numpy as np
import pandas as pd

## write the electron distribution ## 

# Maxwellian parameters
vthe = 10  # Thermal velocity

# Maxwellian distribution function
def f(v):
    return (1 / (vthe * np.sqrt(np.pi))) * np.exp(-v**2 / vthe**2)

# Range and sampling
vmin = 0.0
vmax = 100.0
nPoints = 80

# Discretize velocity range
velocities = np.linspace(vmin, vmax, nPoints + 1)

# Compute weights and normalize
weights_raw = f(velocities)
weights = weights_raw / np.sum(weights_raw)

# Convert velocities to meters/second (from µm/ns → m/s if needed)
# Example scale: if original velocities are in µm/ns, convert to m/s
# Since 1 µm/ns = 10^6 m/s
velocities_m_per_s = velocities * 1e-6

# Generate lines in scientific notation
lines = [
    f"{v:.6e} {w:.6e}"
    for v, w in zip(velocities_m_per_s, weights)
]

# Output file
output_path = "electron_distribution.txt"
with open(output_path, "w") as f_out:
    f_out.write("\n".join(lines))

print(f"Exported {len(lines)} lines to {output_path}")


## write the photon distribution ## 

# # Maxwellian parameters
# vth = 1  # Thermal velocity
# phi = 13.5 #8.14  # Shift (e.g., bandgap energy in eV)

# # Shifted Maxwellian distribution function
# def f(v):
#     return (1 / (vth * np.sqrt(np.pi))) * np.exp(-((v - phi)**2) / vth**2)

# # Range and sampling
# vmin = 10 #0.0
# vmax = 30.0
# nPoints = 80

# # Discretize velocity range
# velocities = np.linspace(vmin, vmax, nPoints + 1)

# # Compute weights and normalize
# weights_raw = f(velocities)
# weights = weights_raw / np.sum(weights_raw)

# # Convert velocities to m/s (if needed) — matching the *10^-6 scaling in Mathematica
# velocities_scaled = velocities * 1e-6

# # Generate GPS-style command lines in scientific notation
# lines = [
#     f"{v:.6e} {w:.6e}"
#     for v, w in zip(velocities_scaled, weights)
# ]

# read in digitize solar spectrum from literature
solardata = pd.read_csv("Fig2-FarrellSolarMinimum.csv")
solar_xdata, solar_ydata = 1240/np.array(solardata["x"]), solardata[" y"]


# Combine into lines: "energy weight" in scientific notation
lines = [
    f"{energy:.6e} {weight:.6e}"
    for energy, weight in zip(solar_xdata*1e-6, solar_ydata)
]

# Export to file
output_path = "photon_distribution.txt"
with open(output_path, "w") as f_out:
    f_out.write("\n".join(lines))

print(f"Exported {len(lines)} lines to {output_path}")