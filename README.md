# Grain-Charging-Simulations

## 1- MATERIALS AND GEOMETRY DEFINITION
 	
## 2- PHYSICS LIST
   
All physics classes are located in PhysicsList.cc. 
> [!IMPORTANT]
> Need to figure out the appropriate physics based on the literature review.
 	 
## 3- THE PRIMARY GENERATOR
 	
## 4-  OUTPUT

Once GUI is closed, the program will generate a .ROOT file. (e.g test.ROOT)
.ROOT file can be read using Jupyter Notebook by the uproot library. 
The following information is saved to the root file:
- pre and post location of particle (primary and other generated particles)
- pre and post kinetic energy of each particle
- particle number
- type of interaction

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
>`gdb ./chargingsphere`
>
> `run`

Once GUI boots:
> `control/execute macro.mac`
	

