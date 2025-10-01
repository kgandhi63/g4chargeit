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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction * Det)
:G4UImessenger(), 
 detector_(Det), rootManager_(G4RootAnalysisManager::Instance()), 
 projectDir_(0), fileNameCmd_(0), PBCCmd_(0), EpsilonCmd_(0), WorldXCmd_(0),WorldYCmd_(0), WorldZCmd_(0), 
 FieldMapStepCmd_(0), RootInputCmd_(nullptr), CADFileCmd_(nullptr), ScaleCmd_(0), FieldFile_(nullptr)
 
{ 
  projectDir_ = new G4UIdirectory("/sphere/");
  projectDir_->SetGuidance("commands specific to this project");
  

  fileNameCmd_ = new G4UIcmdWithAString("/sphere/rootoutput/file",this);
  fileNameCmd_->SetGuidance("Define the filename.");
  fileNameCmd_->SetParameterName("choice",false);
  fileNameCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  PBCCmd_ = new G4UIcmdWithABool("/sphere/PBC",this);
  PBCCmd_->SetGuidance("PBC conditions on or off.");
  PBCCmd_->SetParameterName("choice",false);
  PBCCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  RootInputCmd_ = new G4UIcmdWithAString("/sphere/rootinput/file",this);
  RootInputCmd_->SetGuidance("Root Input Files (space-separated)");
  RootInputCmd_->SetParameterName("choice",false);
  RootInputCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  CADFileCmd_ = new G4UIcmdWithAString("/sphere/cadinput/file",this);
  CADFileCmd_->SetGuidance("CAD Input File.");
  CADFileCmd_->SetParameterName("choice",false);
  CADFileCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  EpsilonCmd_ = new G4UIcmdWithADouble("/sphere/epsilon", this);
  EpsilonCmd_->SetGuidance("Epsilon Value.");
  EpsilonCmd_->SetParameterName("choice",false);
  EpsilonCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  ScaleCmd_ = new G4UIcmdWithADouble("/sphere/cadinput/scale", this);
  ScaleCmd_->SetGuidance("Scale CAD Value.");
  ScaleCmd_->SetParameterName("choice",false);
  ScaleCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  WorldXCmd_ = new G4UIcmdWithADoubleAndUnit("/sphere/worldX", this);
  WorldXCmd_->SetGuidance("Set XY Scale of the World.");
  WorldXCmd_->SetParameterName("choice",false);
  WorldXCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  WorldYCmd_ = new G4UIcmdWithADoubleAndUnit("/sphere/worldY", this);
  WorldYCmd_->SetGuidance("Set XY Scale of the World.");
  WorldYCmd_->SetParameterName("choice",false);
  WorldYCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  WorldZCmd_ = new G4UIcmdWithADoubleAndUnit("/sphere/worldZ", this);
  WorldZCmd_->SetGuidance("Set XY Scale of the World.");
  WorldZCmd_->SetParameterName("choice",false);
  WorldZCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);

  FieldMapStepCmd_ = new G4UIcmdWithADoubleAndUnit("/sphere/fieldMapStep", this);
  FieldMapStepCmd_->SetGuidance("Set step size for field map solver.");
  FieldMapStepCmd_->SetParameterName("choice",false);
  FieldMapStepCmd_->AvailableForStates(G4State_PreInit,G4State_Idle);


  FieldFile_ = new G4UIcmdWithAString("/sphere/field/file",this);
  FieldFile_->SetGuidance("Field Map Save File");
  FieldFile_->SetParameterName("choice",false);
  FieldFile_->AvailableForStates(G4State_PreInit,G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete projectDir_;
  delete fileNameCmd_;
  delete PBCCmd_;
  delete EpsilonCmd_;
  delete ScaleCmd_;
  delete RootInputCmd_;
  delete WorldXCmd_;
  delete WorldYCmd_;
  delete WorldZCmd_;
  delete FieldMapStepCmd_;
  delete FieldFile_;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 

  if( command == fileNameCmd_ )
  {G4cout << "Opening ROOT file: " << newValue << G4endl;
    rootManager_->OpenFile(newValue);}

  if( command == PBCCmd_ )
  { detector_->SetPBC(PBCCmd_->GetNewBoolValue(newValue));}

  if( command == RootInputCmd_ )
  { detector_->SetRootInput(newValue);}

  if( command == CADFileCmd_ )
  { detector_->SetCADFile(newValue);}

  if( command == EpsilonCmd_ )
  { detector_->SetEpsilon(EpsilonCmd_->GetNewDoubleValue(newValue));}

  if( command == ScaleCmd_ )
  { detector_->SetCADScale(ScaleCmd_->GetNewDoubleValue(newValue));}
  
  if( command == WorldXCmd_ )
  { detector_->SetWorldX(WorldXCmd_->GetNewDoubleValue(newValue));}

  if( command == WorldYCmd_ )
  { detector_->SetWorldY(WorldYCmd_->GetNewDoubleValue(newValue));}

  if( command == WorldZCmd_ )
  { detector_->SetWorldZ(WorldZCmd_->GetNewDoubleValue(newValue));}

  if( command == FieldMapStepCmd_ )
  { detector_->SetFieldMapStep(FieldMapStepCmd_->GetNewDoubleValue(newValue));}

  if( command == FieldFile_ )
  { detector_->SetFieldFile(newValue);}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


