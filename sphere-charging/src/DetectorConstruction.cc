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

#include <chrono>  // Make sure this is included

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
#include "SumRadialFieldMap.hh"

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
, PBC_(false), worldX_(0), worldY_(0), worldZ_(0), Epsilon_(0), fieldMinimumStep_(0),sphereSolid_(0),
fieldGradThreshold_(0), CADFile_(""), RootInput_(""), Scale_(1), filename_("")

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

// G4cout << "SiO2 Material: " << SiO2->GetName() << G4endl;
// G4cout << "SiO2 Temperature: " << SiO2->GetTemperature() / kelvin << " K" << G4endl;
// G4cout << "SiO2 Pressure: " << SiO2->GetPressure() / pascal << " Pa" << G4endl;
// G4cout << "SiO2 Density: " << SiO2->GetDensity() / (g/cm3) << " g/cm3" << G4endl;
// G4cout << "SiO2 Ionization Potential: " << SiO2->GetIonisation()->GetMeanExcitationEnergy() / eV << " eV" << G4endl;

// Set Property to SiO2
//G4int ncomponents,natoms; //, abundance;
 
  // // mixture of HDPE and h10BN
  // G4Material* SiO2 = new G4Material("SiO2", 4.78*g/cm3, ncomponents=3,
  //     kStateSolid); //, 293.15*kelvin, 1*atmosphere
  
  // // polyethlyene elements
  // G4Element* Fe = new G4Element("Iron","Fe", 26., 55.845*g/mole);
  // G4Element* Ti = new G4Element("Titanium", "Ti", 22., 47.87*g/mole);
  // G4Element* O = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
 
  // // add components
  // SiO2->AddElement(Fe, natoms=1);
  // SiO2->AddElement(Ti, natoms=1);
  // SiO2->AddElement(O, natoms=3);
  // //SiO2->SetMaterialPropertiesTable(dielectric);
 
  G4bool checkOverlaps = false;
  //     
  // World Characteristics
  //
  // G4double world_sizeX = 900*um;
  // G4double world_sizeY = 900*um;
  // G4double world_sizeZ  = 900*um;


  // define vacuum 
  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  // G4Material* world_mat = new G4Material("Galactic", 1, 1.01*g/mole, universe_mean_density,
  //                 kStateGas, 400*kelvin, 1.e-7*pascal);

  // G4cout << "World Material: " << world_mat->GetName() << G4endl;
  // G4cout << "World Temperature: " << world_mat->GetTemperature() / kelvin << " K" << G4endl;
  // G4cout << "World Pressure: " << world_mat->GetPressure() / pascal << " Pa" << G4endl;
  // G4cout << "World Density: " << world_mat->GetDensity() / (g/cm3) << " g/cm3" << G4endl;
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       worldX_/2, worldY_/2, worldZ_/2);     //its size

  G4cout << "World size: "
       << "X: " << G4BestUnit(worldX_, "Length") << ", "
       << "Y: " << G4BestUnit(worldY_, "Length") << ", "
       << "Z: " << G4BestUnit(worldZ_, "Length") << G4endl;


  logicWorld_ =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld_,           //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  if (PBC_)
  {
    G4PeriodicBoundaryBuilder* pbb = new G4PeriodicBoundaryBuilder();
    logicWorld_ = pbb->Construct(logicWorld_);
    std::cout << "PBC set to logicWorld" << std::endl;
  }

//G4VSolid* sphereSolid_;
 // Load In CAD Files (Check repo for other input files -KG) // Create Input from global variable
if (!CADFile_.empty()) {

  std::string folder_name = "geometry";

  std::ostringstream oss;
  oss << folder_name << "/" << CADFile_;
  std::string full_path = oss.str();

  auto sphere_mesh = CADMesh::TessellatedMesh::FromSTL(full_path);
  sphere_mesh->SetScale(Scale_);
  // Get the solid
  sphereSolid_ = sphere_mesh->GetSolid();

}
 else {
  sphereSolid_ = new G4Sphere("Test", 0., 50*um, 0., 360.*deg, 0., 180.*deg);
  std::cout << "Auto Defaulted To 50 um Sphere." << std::endl;
}

// Create Electrons
if (!RootInput_.empty()) {
    std::istringstream iss(RootInput_);
    std::vector<std::string> file_list;
    std::string file_name;
    TFile* file;
    TTree* tree;

    while (iss >> file_name) {
        file_list.push_back(file_name);
    }

    for (const auto& fil : file_list) {
        std::string folder_name = "root";
        std::ostringstream oss;
        oss << folder_name << "/" << fil;
        std::string full_path = oss.str();

        file = TFile::Open(full_path.c_str(), "READ");
        tree = nullptr;

        if (file && file->IsOpen()) {
            file->GetObject("Hit Data", tree);
            if (tree) {
                std::cout << full_path << " successfully loaded!" << std::endl;
            } else {
                std::cout << "Tree not found in file: " << full_path << std::endl;
                continue;  // skip to next file
            }
        } else {
            std::cout << "Failed to open file: " << full_path << std::endl;
            continue;
        }

        // Branch variables
        int event_number;
        std::vector<double>* post_step_position = nullptr;
        std::vector<double>* pre_step_position = nullptr;
        Char_t volume_name_post[100];
        double kinetic_energy_post_mev;
        double parent_id;
        Char_t particle_type[50];

        // Set branch addresses
        tree->SetBranchAddress("Event_Number", &event_number);
        tree->SetBranchAddress("Post_Step_Position_mm", &post_step_position);
        tree->SetBranchAddress("Pre_Step_Position_mm", &pre_step_position);
        tree->SetBranchAddress("Volume_Name_Post", &volume_name_post);
        tree->SetBranchAddress("Parent_ID", &parent_id);
        tree->SetBranchAddress("Kinetic_Energy_Post_MeV", &kinetic_energy_post_mev);
        tree->SetBranchAddress("Particle_Type", &particle_type);

        // Define the specific volume to filter
        const std::string target_volume = "G4_SILICON_DIOXIDE";

        // Sets to track photon stops and unique holes
        std::set<int> photonStops;
        std::set<int> holeRecordedEvents;

        Long64_t nEntries = tree->GetEntries();
        for (Long64_t i = 0; i < nEntries; i++) {
            tree->GetEntry(i);
            if (!post_step_position || post_step_position->size() < 3) continue;
            if (!pre_step_position || pre_step_position->size() < 3) continue;


            std::string ptype = particle_type;

            // Track photon stops (photon reaches zero kinetic energy)
            if (ptype == "gamma" && kinetic_energy_post_mev == 0.0 && std::string(volume_name_post) == target_volume) {
                photonStops.insert(event_number);
            }

            // Stopped electrons
            if (ptype == "e-" && kinetic_energy_post_mev == 0.0 && std::string(volume_name_post) == target_volume) {
                G4ThreeVector pos((*post_step_position)[0] * mm,
                                  (*post_step_position)[1] * mm,
                                  (*post_step_position)[2] * mm);
                fElectronPositions.push_back(pos);
            }

            // Stopped protons
            if (ptype == "proton" && kinetic_energy_post_mev == 0.0 && std::string(volume_name_post) == target_volume) {
                G4ThreeVector pos((*post_step_position)[0] * mm,
                                  (*post_step_position)[1] * mm,
                                  (*post_step_position)[2] * mm);
                fProtonPositions.push_back(pos);
            }

            // Hole = first secondary electron (parent_id == 1) in event where photon stopped
            if (ptype == "e-" && parent_id == 1 && photonStops.count(event_number)) {
                if (holeRecordedEvents.insert(event_number).second) {
                    G4ThreeVector pos((*pre_step_position)[0] * mm,
                                      (*pre_step_position)[1] * mm,
                                      (*pre_step_position)[2] * mm);
                    fHolePositions.push_back(pos);
                }
            }
        }

        // Print results per file
        G4cout << "File: " << full_path << G4endl;
        G4cout << "  Electrons: " << fElectronPositions.size() << G4endl;
        G4cout << "  Protons:   " << fProtonPositions.size() << G4endl;
        G4cout << "  Holes:     " << fHolePositions.size() << G4endl;

        file->Close();
    }
}


G4LogicalVolume*logicSphere= new G4LogicalVolume(sphereSolid_, SiO2 , SiO2->GetName());  


new G4PVPlacement(0,                          	//no rotation
              G4ThreeVector(0,0,0),     //at (0,0,0)
              logicSphere,                               //its logical volume
              SiO2->GetName(),               //its name
              logicWorld_,                        //its mother volume
              false,                              //no boolean operation
              0);                                 //copy number
              

//logicWorld_->SetVisAttributes(G4VisAttributes::GetInvisible());

//G4double step = 1 * nm;
//G4UserLimits* userLimits = new G4UserLimits(step);
//logicSphere->SetUserLimits(userLimits);

return physWorld;
}

void DetectorConstruction::ConstructSDandField() {
  static auto sdManager = SDManager::GetInstance();
  sdManager->CreateSD();
  auto sd = sdManager->GetSD();
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);

  // Combine all positions and charges
  std::vector<G4ThreeVector> allPositions;
  std::vector<G4double> allCharges;

  // Electrons (negative charge)
  G4double eCharge = -1.602e-19 * CLHEP::coulomb;
  for (const auto& pos : fElectronPositions) {
    allPositions.push_back(pos);
    allCharges.push_back(eCharge);
  }
  // Protons (positive charge)
  G4double pCharge = +1.602e-19 * CLHEP::coulomb;
  for (const auto& pos : fProtonPositions) {
    allPositions.push_back(pos);
    allCharges.push_back(pCharge);
  }
  // Holes (positive charge)
  G4double hCharge = +1.602e-19 * CLHEP::coulomb;
  for (const auto& pos : fHolePositions) {
    allPositions.push_back(pos);
    allCharges.push_back(hCharge);
  }

  G4cout << "Starting Adaptive Field Map Precomputation" << G4endl;

  // Start timer
  auto start = std::chrono::high_resolution_clock::now();

  // Define grid: 800 µm cube centered at origin, 2 µm step
  G4ThreeVector min(-worldX_/2, -worldY_/2, -worldZ_/2); //-400*um, -400*um, -400*um);
  G4ThreeVector max(worldX_/2, worldY_/2, worldZ_/2); // 400*um,  400*um,  400*um);
  G4ThreeVector step(10*um, 10*um, 10*um);

  // First create the uniform field map
  auto uniformFieldMap = new SumRadialFieldMap(allPositions, allCharges,
                                              min, max, step, "uniform_field_map.bin",
                                              SumRadialFieldMap::StorageType::Double);

  // Then create adaptive field map using the uniform one
  auto adaptiveFieldMap = new AdaptiveSumRadialFieldMap(
      std::unique_ptr<G4VSolid>(sphereSolid_),
      *uniformFieldMap,  // Pass the uniform field map
      fieldGradThreshold_,               // Gradient threshold (V/m per micron)
      fieldMinimumStep_,
      filename_,
      min, max,
      6,                 // max depth
      AdaptiveSumRadialFieldMap::StorageType::Double
  );

  // End timer
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  G4cout << "Ending Adaptive Field Map Precomputation" << G4endl;

  // Convert seconds to minutes (1 minute = 60 seconds)
  double duration_in_minutes = duration.count() / 60.0;
  G4cout << "Precomputation took " << duration_in_minutes << " minutes." << G4endl;

  // Set up the field manager - NOW IT WILL WORK!
  auto worldFM = new G4FieldManager();
  worldFM->SetDetectorField(adaptiveFieldMap);  // No error now!
  worldFM->SetMinimumEpsilonStep(1.0e-7);
  worldFM->SetMaximumEpsilonStep(1.0e-4);
  worldFM->SetDeltaOneStep(0.1*um);

  // This will also work now!
  auto equation = new G4EqMagElectricField(adaptiveFieldMap);
  const G4int nvar = 8;
  auto stepper = new G4DormandPrince745(equation, nvar);
  auto driver  = new G4IntegrationDriver<G4DormandPrince745>(0.1*um, stepper, nvar);
  auto chord   = new G4ChordFinder(driver);
  worldFM->SetChordFinder(chord);

  logicWorld_->SetFieldManager(worldFM, true);

  // G4cout << "Starting Field Map Precomputation" << G4endl;

  // // Start timer
  // auto start = std::chrono::high_resolution_clock::now();

  // // Define grid: 800 µm cube centered at origin, 2 µm step
  // G4ThreeVector min(-worldX_/2, -worldY_/2, -worldZ_/2); //-400*um, -400*um, -400*um);
  // G4ThreeVector max(worldX_/2, worldY_/2, worldZ_/2); // 400*um,  400*um,  400*um);

  // // // Create the precomputed field map
  // // auto sumField = new SumRadialFieldMap(allPositions, allCharges,
  // //                                       min, max, step, filename_,
  // //                                       SumRadialFieldMap::StorageType::Double);



  // // End timer
  // auto end = std::chrono::high_resolution_clock::now();
  // std::chrono::duration<double> duration = end - start;
  // G4cout << "Ending Field Map Precomputation" << G4endl;

  // // Convert seconds to minutes (1 minute = 60 seconds)
  // double duration_in_minutes = duration.count() / 60.0;
  // G4cout << "Precomputation took " << duration_in_minutes << " minutes." << G4endl;

  // // Set up the field manager as before
  // auto worldFM = new G4FieldManager();
  // worldFM->SetDetectorField(adaptiveSumField);
  // worldFM->SetMinimumEpsilonStep(1.0e-7);
  // worldFM->SetMaximumEpsilonStep(1.0e-4);
  // worldFM->SetDeltaOneStep(0.1*um);

  // auto equation = new G4EqMagElectricField(adaptiveSumField);
  // const G4int nvar = 8;
  // auto stepper = new G4DormandPrince745(equation, nvar);
  // auto driver  = new G4IntegrationDriver<G4DormandPrince745>(0.1*um, stepper, nvar);
  // auto chord   = new G4ChordFinder(driver);
  // worldFM->SetChordFinder(chord);

  // logicWorld_->SetFieldManager(worldFM, true);

  // Attach sensitive detector to daughters
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

void DetectorConstruction::SetWorldX(G4double value)
{
  worldX_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetWorldY(G4double value)
{
  worldY_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetWorldZ(G4double value)
{
  worldZ_ = value;
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

void DetectorConstruction::SetFieldFile(G4String value)
{
  filename_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetFieldMinimumStep(G4double value)
{
  fieldMinimumStep_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetFieldGradThreshold(G4double value)
{
  fieldGradThreshold_ = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
