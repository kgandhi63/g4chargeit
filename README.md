# Description

This package contains a high-performance Monte Carlo simulation framework based on the open source package *Geant4* for modeling the time-dependent electrostatic charging of dielectric materials. This document assumes that you are familiar with developing applications based on the Geant4 Monte Carlo toolkit using a linux environment.  


The G4ChargeIt package adds the functionality of iterative event based simulations to the Geant4 Monte Carlo toolkit.

The following is accomplished with the G4ChargeIt module:
* iterations of simulations are generated and run sequentially
* a general electric field solver is natively implemented to model electric charging of dielectric materials
* time steps are discretized baesd on the relative fluence of incident particle distributions
* geometry surface charge  is conserved and carried over to each submitted iteration
* provides an analysis tool kit to demonstrate the charging of arbitrary geometric configurations



# Dependancies
Ensure the following packages are installed: 
> `gcc/12.3.0`

> `cmake/3.11.3`

> `root/6.30.04`

> `xerces-c/3.30.2`

> `openmpi/4.1.5`

> `geant4/11.3.0`

# Obtaining Source Code

Clone this repository with the following command:

> `git clone https://github.com/avira7/Grain-Charging-Simulations.git`

# Environment Setup
Before compiling, you must ensure that you have linked periodic boundary conditions in Geant4. 

Installation instructions can be found here: 

> `https://github.com/amentumspace/g4pbc`

Then export the installation in your bash script as a global variable:

> `export G4PBC=/path/to/g4pbc/installation`

# Build & Run
The simulation is built using the CMake compiler. The source code is located in the `sphere-charging` directory. 

Configure the run-time environment by sourcing the configuration script of your geant4 installation, then perform an out-of-source build:

> `source <path_to_geant4_install>/bin/geant4.sh`

> `mkdir build`

> `cd build/`

Run the following `cmake` command with the appropriate tags to configure the project with MPI and OpenMP support:

> `cmake -Dg4pbc_DIR=$G4PBC -DCMAKE_CXX_COMPILER=$(which mpicxx) -DCMAKE_CXX_FLAGS="-fopenmp" ../g4chargeit/`

Once compiled, edit or create a new submission python script with your desired parameters. CAD files of arbitarary geometries can be used in STL ASCII format. All source files have already been explicitly included in CMakeLists.txt.

Compile the code using `make`. The `-j` flag enables parallel compilation to speed up the process.

> `make -j<number_processors>`

A `submit_iterations.sh` script will appear, simply run:

>  `./submit_iterations.sh`

This will start your thread of simulations that can be viewed in output logs. 
## Example Application 
An application is already provided. Submission scripts are generated for the charging of lunar regolith in the presence of photoelectrons and solar wind. Our example assumes the simulations are being run on a high performance cluster using the SLURM job scheduling system. The exact HPC batch commands will need to be change to reflect your computing environment. 

To run the example submit before running `make`:

> `python create_submission_file.py`

Then run `make` and submit: 

>  `./submit_iterations.sh`

Three cases will be run: solar wind irradiation, photoemission, and a combination of the two.

## Analysis

Analysis tools are provided in the `analysis` folder. The conventional file output type is in ROOT format. 

