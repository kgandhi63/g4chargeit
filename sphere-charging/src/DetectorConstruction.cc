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

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"



#include "G4MaterialPropertiesTable.hh"
#include "G4FieldManager.hh"
#include "G4ThreeVector.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh" 
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
#include "G4DormandPrince745.hh"
#include "G4IntegrationDriver.hh"  
#include "G4UniformElectricField.hh"
#include "G4SystemOfUnits.hh"
#include "SDManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4Element.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "SumRadialField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"

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
, PBC_(false), Epsilon_(0), CADFile_(""), RootInput_(""), Scale_(1)

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

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes() {
G4GeometryManager::GetInstance()->OpenGeometry();
G4PhysicalVolumeStore::GetInstance()->Clean();
G4LogicalVolumeStore::GetInstance()->Clean();
G4SolidStore::GetInstance()->Clean();

// Add dielectric Constant
 G4MaterialPropertiesTable* dielectric = new G4MaterialPropertiesTable();
 dielectric->AddConstProperty("Epsilon",  Epsilon_, true);

// Set Property to SiO2
G4Material* SiO2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
SiO2->SetMaterialPropertiesTable(dielectric);
 
G4bool checkOverlaps = false;
  //     
  // World Characteristics
  //
  G4double world_sizeX = 200*um;
  G4double world_sizeY = 200 *um;
  G4double world_sizeZ  = 200*um;


  // define vacuum 
  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       world_sizeX/2, world_sizeY/2, world_sizeZ/2);     //its size

  logicWorld_ =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  if (PBC_)
   {
    G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
      logicWorld_ = pbb->Construct(logicWorld_);
    }


  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld_,           //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking



G4VSolid* sphere_solid;
 // Load In CAD Files (Check repo for other input files -KG) // Create Input from global variable
if (!CADFile_.empty()) {

  std::string folder_name = "geometry";

  std::ostringstream oss;
  oss << folder_name << "/" << CADFile_;
  std::string full_path = oss.str();

  auto sphere_mesh = CADMesh::TessellatedMesh::FromSTL(full_path);
  sphere_mesh->SetScale(Scale_);
  // Get the solid
  sphere_solid = sphere_mesh->GetSolid();
 }
 else {
  sphere_solid = new G4Sphere("Test", 0., 50*um, 0., 360.*deg, 0., 180.*deg);
  std::cout << "Auto Defaulted To 50 um Sphere." << std::endl;
  
 }

//Create Electrons

if (!RootInput_.empty()) {
  std::istringstream iss(RootInput_);
  std::vector<std::string> file_list;
  std::string file_name;
  TFile* file;
  TTree *tree;
    
  while (iss >> file_name) {
    file_list.push_back(file_name);
  }
  
  
  for (const auto& fil : file_list) {
    std::string folder_name = "root";
  
    std::ostringstream oss;
    oss << folder_name << "/" << fil;
    std::string full_path = oss.str();

  
    file = TFile::Open(full_path.c_str(),"READ");
    tree = nullptr;

    if (file && file->IsOpen()) {
        file->GetObject("Hit Data", tree);
        if (tree) {
            std::cout << file << " successfully loaded!" << std::endl;
        } else {
            std::cout << "Tree not found in file." << std::endl;
        }
    }

    file->GetObject("Hit Data", tree); 

    // Declare pointers for branches
    std::vector<double>* post_step_position = nullptr;
    Char_t volume_name_post[100];
    double kinetic_energy_post_mev;
    // Set branch addresses
    tree->SetBranchAddress("Post_Step_Position_mm", &post_step_position);
    tree->SetBranchAddress("Volume_Name_Post", &volume_name_post);
    tree->SetBranchAddress("Kinetic_Energy_Post_MeV", &kinetic_energy_post_mev);
    //tree->SetBranchAddress("Particle_ID", );

    // Define the specific volume to filter
    const std::string target_volume = "G4_SILICON_DIOXIDE";  // Replace with the desired volume name

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        if (volume_name_post && std::string(volume_name_post) == target_volume && kinetic_energy_post_mev == 0.0) {
          if (post_step_position && post_step_position->size() >= 3) { 
              // Ensure the vector contains at least x, y, z components
              G4ThreeVector pos((*post_step_position)[0] * mm, // x
                                (*post_step_position)[1] * mm, // y
                                (*post_step_position)[2] * mm); // z
              fElectronPositions.push_back(pos);
        }
      }
    }
    G4Sphere * electron = new G4Sphere("Electron", 0., 1*nm , 0., 360.*deg, 0., 180.*deg);
    G4LogicalVolume*logicElectron = new G4LogicalVolume(electron, world_mat, electron->GetName());
    logicElectron->SetVisAttributes(G4VisAttributes::GetInvisible());
    int count = 0;
    // Use these locations for electron placements
    for (const auto& pos : fElectronPositions) {
      new G4PVPlacement(
          0,                      // no rotation
          pos,                    // position from the vector
          logicElectron,            // logical volume (or particle placement logic)
          "ElectronPlacement",    // name
          logicWorld_,            // mother volume
          false,                  // no boolean operation
          0                       // copy number
      );

    G4cout << "Placed Electron: " << count++ << G4endl;
    }

}
}

G4LogicalVolume*logicSphere= new G4LogicalVolume(sphere_solid, SiO2 , SiO2->GetName());  


new G4PVPlacement(0,                          	//no rotation
              G4ThreeVector(),     //at (0,0,0)
              logicSphere,                               //its logical volume
              SiO2->GetName(),               //its name
              logicWorld_,                        //its mother volume
              false,                              //no boolean operation
              0);                                 //copy number



//logicWorld_->SetVisAttributes(G4VisAttributes::GetInvisible());



// Set Maximum Step Size (to avoid RungeKutta Errors)
G4double step = 1 * um;
G4UserLimits* userLimits = new G4UserLimits(step);
logicWorld_->SetUserLimits(userLimits);

return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructSDandField() {
  static auto sdManager = SDManager::GetInstance();
  sdManager->CreateSD();
  auto sd = sdManager->GetSD();
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);

  // 2) build one SumRadialField from all stored positions:
  G4double eCharge = -1.602e-19 * CLHEP::coulomb;
  auto sumField = new SumRadialField(fElectronPositions, eCharge);

  // 3) one global FieldManager on the world:
  auto worldFM = new G4FieldManager();
  worldFM->SetDetectorField(sumField);
  worldFM->SetMinimumEpsilonStep(1.0e-7);
  worldFM->SetMaximumEpsilonStep(1.0e-4);
  worldFM->SetDeltaOneStep(0.1*um);

  auto equation = new G4EqMagElectricField(sumField);
  const G4int nvar = 8;
  auto stepper = new G4DormandPrince745(equation, nvar);
  auto driver  = new G4IntegrationDriver<G4DormandPrince745>(0.1*um, stepper, nvar);
  auto chord   = new G4ChordFinder(driver);
  worldFM->SetChordFinder(chord);

  logicWorld_->SetFieldManager(worldFM, true);

  G4int nD = logicWorld_->GetNoDaughters();
  for (G4int i = 0; i < nD; ++i) {
    auto lv = logicWorld_->GetDaughter(i)->GetLogicalVolume();
    if (sd) {
      lv->SetSensitiveDetector(sd);
    }
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetPBC(G4bool value)
{
  PBC_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

 void DetectorConstruction::SetEpsilon(G4double value)
{
  Epsilon_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
 

void DetectorConstruction::SetRootInput(G4String value)
{
  RootInput_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
 

void DetectorConstruction::SetCADFile(G4String value)
{
  CADFile_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
 
void DetectorConstruction::SetCADScale(G4double value)
{
  Scale_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}