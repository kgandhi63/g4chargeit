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
/// \file charging_sphere.cc
/// \brief Main program 
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Types.hh"
#include "G4MTRunManager.hh"
#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParticleHPManager.hh"
#include "G4PeriodicBoundaryPhysics.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmParameters.hh"
#include "G4EmLivermorePhysics.hh"

#include "G4ParticleDefinition.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4PhysicsListHelper.hh" 
#include "G4StepLimiterPhysics.hh"
#include "G4ProductionCutsTable.hh"

#include "G4LindhardSorensenIonModel.hh"
#include "G4hIonisation.hh"
#include "G4Proton.hh"

#include <omp.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  /// detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;
  if (argc == 1) ui = new G4UIExecutive(argc,argv);

  /// choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // --------------------------------------------------------------------
  // MPI session
  // --------------------------------------------------------------------

  #ifdef _OPENMP
      G4cout << "=== OpenMP Configuration ===" << G4endl;
      G4cout << "OpenMP is enabled. Version: " << _OPENMP << G4endl;
      G4cout << "Max threads available: " << omp_get_max_threads() << G4endl;
  #else
      G4cout << "OpenMP is NOT enabled" << G4endl;
  #endif

  // --------------------------------------------------------------------
  // user application setting
  // --------------------------------------------------------------------

  /// construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // #ifdef G4MULTITHREADED
  //   G4MTRunManager* runManager = new G4MTRunManager;
  //   G4int numThreads = 2*(G4Threading::G4GetNumberOfCores());
  //   runManager->SetNumberOfThreads(numThreads);
  //   G4cout << "Multithreaded - Number of threads: " << numThreads << G4endl;
  //   G4cout << "Number of cores: " << G4Threading::G4GetNumberOfCores() << G4endl;
  // #else
  //   G4RunManager* runManager = new G4RunManager;
  //   G4cout << "Single threaded" << G4endl;
  // #endif

  /// set mandatory initialization classes
  DetectorConstruction* det= new DetectorConstruction;
  runManager->SetUserInitialization(det);

  // doesn't do anything ...
  //G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(10*eV, 10*keV); 

  // Use PhysListFactory to get a reference physics list
  G4PhysListFactory factory; 
   G4VModularPhysicsList* physList=factory.GetReferencePhysList("FTFP_BERT_EMX");
   // already included in FTFP_BERT_EMX: G4EmExtraPhysics, G4hImpactIonisation, G4IonQMDPhysics

   //physList->RegisterPhysics(new G4StepLimiterPhysics); 
   //physList->ReplacePhysics(new G4EmStandardPhysicsWVI);
   //physList->ReplacePhysics(new G4EmLowEPPhysics);
   //physList->ReplacePhysics(new G4EmLivermorePhysics);
   physList->ReplacePhysics(new G4EmStandardPhysics_option4);

  G4EmParameters* emParams = G4EmParameters::Instance();
  emParams->SetFluo(true);   // Enable fluorescence
  emParams->SetAuger(true);  // Enable Auger electrons
  emParams->SetPixe(true);   // Enable PIXE
  //emParams->SetQuantumEntanglement(true); // Enable quantum entanglement

  // G4ProcessManager* pmanager = G4Proton::Proton()->GetProcessManager();
  // G4hIonisation* hIoni = new G4hIonisation();
  // G4VEmModel* mod = new G4LindhardSorensenIonModel();
  // hIoni->SetEmModel(mod);
  // pmanager->AddProcess(hIoni);

  // // Add step limiter to all particles
  // auto* stepLimitPhys = new G4StepLimiterPhysics();
  // stepLimitPhys->SetApplyToAll(true); // apply to all particles, not just charged
  // physList->RegisterPhysics(stepLimitPhys);

  // Add periodic boundary conditions
  G4PeriodicBoundaryPhysics* pbc = new G4PeriodicBoundaryPhysics("PBC", true, true, false); // Turn off pbc in Z direction
  pbc->SetVerboseLevel(0);
  physList->RegisterPhysics(pbc);

  runManager->SetUserInitialization(physList);
  runManager->SetUserInitialization(new ActionInitialization());

  /// initialize visualization
  G4VisManager* visManager = nullptr;

  /// Replaced HP environmental variables with C++ calls
  G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes( false );
  G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState( false );
  G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation( false );
  G4ParticleHPManager::GetInstance()->SetNeglectDoppler( false );
  G4ParticleHPManager::GetInstance()->SetProduceFissionFragments( false );
  G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel( false );
  G4ParticleHPManager::GetInstance()->SetUseNRESP71Model( false );
  //G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes( true );
  //G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState( true );
  //G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation( true );
  //G4ParticleHPManager::GetInstance()->SetNeglectDoppler( true );
  //G4ParticleHPManager::GetInstance()->SetProduceFissionFragments( true );
  //G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel( true );
  //G4ParticleHPManager::GetInstance()->SetUseNRESP71Model( true );

  ///get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (ui)  {
    /// interactive mode
    visManager = new G4VisExecutive;
    visManager->Initialize();
    ui->SessionStart();
    delete ui;
  }
  else  {
    /// batch mode
    G4String command1 = "/control/macroPath macros/";   
    G4String command2 = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command1+fileName);
    UImanager->ApplyCommand(command2+fileName);
  }

  // job termination
  delete visManager;
  delete runManager;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......