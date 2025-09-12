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
/// \file PhysicsList.cc
/// \brief Implementation of the PhysicsList class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"
//#include "PhysListEmLivermore.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4String.hh"

// physics models

#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4HadronElasticPhysics_mod.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4PenelopeComptonModel.hh"

#include "G4NuclideTable.hh"
#include "G4NuclearLevelData.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4PeriodicBoundaryPhysics.hh"



//#include "G4PeriodicBoundaryPhysics.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList()
:G4VModularPhysicsList()
{
  G4int verb = 1; // changed verbose level
  SetVerboseLevel(0);

  // 
  // Decay
  RegisterPhysics(new G4DecayPhysics());
  RegisterPhysics(new G4RadioactiveDecayPhysics());

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics_option4());
  // EM physics
  //RegisterPhysics(new PhysListEmLivermore(name));
  RegisterPhysics(new G4EmPenelopePhysics());
  //RegisterPhysics(new G4PenelopeComptonModel());
  RegisterPhysics( new G4StoppingPhysics(1));
  RegisterPhysics(new G4EmExtraPhysics());
  RegisterPhysics(new G4IonPhysics());
  //RegisterPhysics(new G4EmExtraPhysics());

  // 
  // Other Physics Libraries to include
  //

  //Mandatory for G4NuclideTable
  //Half-life threshold must be set small or many short-lived isomers
  //will not be assigned life times (default to 0)
  //CDR: I took a look at the code and I think setting the value negative will
  //force a read of all loaded isotopes.
  //G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(-0.1*picosecond);
  //G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);

  //read new PhotonEvaporation data set
  //G4DeexPrecoParameters* deex = G4NuclearLevelData::GetInstance()->GetParameters();
  //deex->SetCorrelatedGamma(true);
  //deex->SetStoreAllLevels(true);
  //deex->SetMaxLifeTime(G4NuclideTable::GetInstance()->GetThresholdOfHalfLife()/std::log(2.));

  // step limiter physics
  RegisterPhysics(new G4StepLimiterPhysics());


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
