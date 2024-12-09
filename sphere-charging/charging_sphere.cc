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
/// \file hBNdet.cc
/// \brief Main program of hBNdet project
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "G4Types.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif



#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "G4ParticleHPManager.hh"
#include "G4EmPenelopePhysics.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PeriodicBoundaryPhysics.hh"


//#include "G4PeriodicBoundaryPhysics.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  /// detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;
  if (argc == 1) ui = new G4UIExecutive(argc,argv);

  /// choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  /// construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  /// set mandatory initialization classes
  DetectorConstruction* det= new DetectorConstruction;
  runManager->SetUserInitialization(det);

  // EITHER CLOSE THE PHYSICSLIST THAT WE DEFINE OR THE REFERENCE PHYSICS LIST

  // load all physics that we defined
  //PhysicsList* phys = new PhysicsList;

  // use the reference physics list <-- this only works for version 11.2.0
 G4PhysListFactory factory;
 G4VModularPhysicsList* physList=factory.GetReferencePhysList("FTFP_BERT");
 physList->ReplacePhysics(new G4EmPenelopePhysics);

  //arguments: physics list name, cycle x, y, z, reflecting walls
  G4PeriodicBoundaryPhysics* pbc = new G4PeriodicBoundaryPhysics("PBC", true, true,false);
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

  /// job termination
  delete visManager;
  delete runManager;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
