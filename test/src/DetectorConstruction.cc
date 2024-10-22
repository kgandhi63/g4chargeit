//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "G4VUserDetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "SDManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4Element.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4UserLimits.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4UniformElectricField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4NistManager.hh"
#include "G4PeriodicBoundaryBuilder.hh"
#include "G4UserLimits.hh"

#include "CADMesh.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction()
, PBC_(0)

{
  // create commands for interactive definition of the detector 
  detectorMessenger_ = new DetectorMessenger(this);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // Define Materials
  //

 G4Material* SiO2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

  
  //     
  // World
  //
  G4double world_sizeXY = 100*um;
  G4double world_sizeZ  = 100*um;


  // define vacuum 
  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       world_sizeXY/2, world_sizeXY/2, world_sizeZ/2);     //its size

     logicWorld_ =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  //Create Periodic Boundary Condictions


   G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld_,           //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // Set vacuum layer prior to layered material
  //
  if (PBC_)
   {
    G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
      logicWorld_ = pbb->Construct(logicWorld_);
    }


 // Load In CAD Files
  auto sphere_mesh = CADMesh::TessellatedMesh::FromSTL("30.STL");
  sphere_mesh->SetScale(.0025);
  // Get the solid
 auto sphere_solid = sphere_mesh->GetSolid();
/*auto tets = CADMesh::TetrahedralMesh::FromPLY("assembly.PLY");

  tets->SetMaterial(SiO2);

  auto assembly = tets->GetAssembly();

  auto position = G4ThreeVector();
  auto rotation = new G4RotationMatrix();

  assembly->MakeImprint(logicWorld_, position, rotation);
*/
  //     
  // 100 um sphere
 // G4Sphere * sphere_solid = new G4Sphere("SphereSolid", 0., 50*um, 0., 360.*deg, 0., 180.*deg);



    // Create logical volume
  G4LogicalVolume* logicSphere = new G4LogicalVolume(sphere_solid, SiO2 , SiO2->GetName());  
  
  new G4PVPlacement(0,                          	//no rotation
                    G4ThreeVector(-37.5*um,-37.5*um,-37.5*um),     //at (0,0,0)
                    logicSphere,                               //its logical volume
                    SiO2->GetName(),               //its name
                    logicWorld_,                        //its mother volume
                    false,                              //no boolean operation
                    0);                                 //copy number
  
  // Set vacuum layer after to layered material
  //

  // place the mother volume

  

  // colors
  logicWorld_->SetVisAttributes(G4VisAttributes::GetInvisible());


 
 G4double step = 1 *um;
 G4UserLimits* userLimits = new G4UserLimits(step);
 logicWorld_->SetUserLimits(userLimits);

  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField() {

  static auto sdManager = SDManager::GetInstance();
  sdManager -> CreateSD();

  // create an instance of a sensitive detector
  SensitiveDetector* sd = sdManager -> GetSD();
  
  // register the sensitive detector to the SD manager  
  G4SDManager* g4sdManager = G4SDManager::GetSDMpointer();  
  g4sdManager ->AddNewDetector(sd);
  
  // assign the pointer of your SD to the logical volumne of your detector geometry


  int numDaughters = logicWorld_->GetNoDaughters();

  G4cout << numDaughters << G4endl;
  for (int i=0; i<numDaughters; i++) {
        G4LogicalVolume* logicLayer = logicWorld_-> GetDaughter(i) -> GetLogicalVolume();
        G4cout << logicLayer->GetName() << G4endl;
        logicLayer-> SetSensitiveDetector(sd); 
  }

  //logicWorld_ -> SetSensitiveDetector(sd);
  G4cout << "Set Area to Sensitive Detector" << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void DetectorConstruction::SetDensity(G4double value)
{
  density_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetPBC(G4bool value)
{
  PBC_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
