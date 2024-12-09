#ifndef G4RadialElectricField_h
#define G4RadialElectricField_h

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"

class G4RadialElectricField : public G4ElectricField {
public:
    G4RadialElectricField();
    virtual ~G4RadialElectricField(); 

    // Override GetFieldValue
    void GetFieldValue(const G4double Point[4], G4double* Efield) const override;


};


#endif
