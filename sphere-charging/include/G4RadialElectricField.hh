#ifndef G4RadialElectricField_h
#define G4RadialElectricField_h

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"

class G4RadialElectricField : public G4ElectricField {
public:
    // Set up Field Parameter
    G4RadialElectricField(double field);
    virtual ~G4RadialElectricField(); 
    
    // Override GetFieldValue
    void GetFieldValue(const G4double Point[4], G4double* Efield) const override;
private:
    double field_;
};


#endif
