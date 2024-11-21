# Grain-Charging-Simulations

## 1- MATERIALS AND GEOMETRY DEFINITION
 
It is a single box representing a homogeneous medium.
Two parameters define the material:
- material,
- physical dimensions,
- unit cell vectors,
- space groups,
- form factors,
- crystal orientation.

The geometry (e.g. 2 mm silicon crystal) is built in 
DetectorConstruction, but the above parameters can be changed interactively 
via commands defined in DetectorMessenger.
 	
## 2- PHYSICS LIST
   
All physics classes are located in PhysicsList.cc. 
> [!IMPORTANT]
> Need to figure out the appropriate physics based on the literature review.
 	 
## 3- THE PRIMARY GENERATOR
 
The primary kinematic is a single particle randomly shot at the center of the box. 
The type of the particle and its energy are set in PrimaryGeneratorAction (e.g., proton @ 100 GeV), 
and can be changed via macro file in example.mac. The primary particles are uniformly distributed 
and incident normal to the surface of the material.
 	
## 4-  OUTPUT

Once GUI is closed, the program will generate a .ROOT file. (e.g test.ROOT)
.ROOT file can be read using Jupyter Notebook by the uproot library. 
The following information is saved to the root file:
- pre and post location of particle (primary and other generated particles)
- pre and post kinetic energy of each particle
- particle number
- type of interaction

> [!IMPORTANT]
> Need to determine if there is other information to save to obtain information about PKAs.
 	 				
## 5- VISUALIZATION
 
The Visualization Manager is set in vis.mac.
The initialization of the drawing is done via the commands
/vis/... in the macro vis.mac. To get visualisation:
> `/control/execute vis.mac`

The detector has a default view which is a longitudinal view of the box.
The tracks are drawn at the end of event, and erased at the end of run.
	
## 6- HOW TO START ?

Execute `cmake` in build folder. Run the following commands:

>`cmake *folder path* `
>
>`make`
>
>`gdb ./CrystalProject`
>
> `run`

Once GUI boots:
> `control/execute example.mac`
	
Macros provided in example.mac: proton (100 GeV) in silicon.

