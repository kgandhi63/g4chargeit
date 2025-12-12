# Description

This package contains a Monte Carlo simulation framework based on the open source package Geant4 for modeling the time-dependent electrostatic charging of dielectric materials. This document assumes that you are familiar with developing applications based on the Geant4 Monte Carlo toolkit using a linux environment.  

The g4chargeit package adds the functionality of iterative event based simulations to the Geant4 Monte Carlo toolkit.

The following is accomplished with the g4chargeit module:
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

An executable called `g4chargeit` will appear, simply run:

>  `./g4chargeit`

A single iteration can be run using `/control/execute test-macros/testphotons-regular.mac`.

## Example Application 
An application is provided. Submission scripts are generated for the charging of lunar regolith in the presence of photoelectrons and solar wind. Our example assumes the simulations are being run on a high performance cluster using the SLURM job scheduling system. The exact HPC batch commands will need to be change to reflect your computing environment. 

Two cases will be run, solar wind irradiation and photons. To create macros for a series of iterations, we first need to run the 0th iteration:

> `python createMacros-RegularSpheres.py`
> 
> `sbatch batchscripts/000_iteration0_onlysolarwind_num100000.root`
> 
> `sbatch batchscripts/000_iteration0_onlyphotoemission_num100000.root`

Then recreate all macro files:

> `python createMacros-RegularSpheres.py`

Multiple jobs can be submitted through the HPC using:

> `python create_submission_file.py [startIterationNum] [endIterationNum] [irradiationCase]`
>
>  `./submit_iterations.sh`

This will start your thread of simulations that can be viewed in the `outputlogs`. 


## Analysis

Analysis tools are provided in the `analysis` folder. The conventional file output type is in ROOT format. 

