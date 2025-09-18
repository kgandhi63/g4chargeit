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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4UniformElectricField.hh"
#include "G4FieldManager.hh"
#include "SumRadialField.hh"

#include <memory>

class G4LogicalVolume;
class DetectorMessenger;
class G4VPhysicalVolume;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();

    // construct function
    virtual G4VPhysicalVolume* Construct();
    // function to set the sensitive detector volume
    void SetFieldValue(G4ThreeVector value);
    void ConstructSDandField() override;
    void SetPBC (G4bool);
    void SetRoot (G4bool);
    void SetEpsilon (G4double);
    void SetCADFile (G4String);
    void SetRootInput (G4String);
    void SetCADScale (G4double);

    void SetWorldXY (G4double);
    void SetWorldZ (G4double);
                       
  private:
    G4VPhysicalVolume* ConstructVolumes();  
    G4bool PBC_;
    G4double worldXY_;
    G4double worldZ_;
    G4LogicalVolume* logicWorld_; 
    G4double Epsilon_;
    G4String CADFile_;
    G4String RootInput_;
    G4double Scale_;
    std::vector<G4ThreeVector> fHolePositions;
    std::vector<G4ThreeVector> fElectronPositions;
    std::vector<G4ThreeVector> fProtonPositions;
    DetectorMessenger* detectorMessenger_;

};



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
