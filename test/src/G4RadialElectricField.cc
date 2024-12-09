#include "G4RadialElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

// Constructor
G4RadialElectricField::G4RadialElectricField(){
    // Initialization code, if necessary
}

// Override GetFieldValue to create a radially varying field
void G4RadialElectricField::GetFieldValue(const G4double Point[4], G4double* Efield) const {
    // Position vector in 3D space
    G4ThreeVector position(Point[0], Point[1], Point[2]);
    G4double radius = position.mag();

    // Define radial field strength, avoiding division by zero
    G4double fieldStrength = (radius > 0) ? (1440*volt/um) / radius : 0.0;

    // Calculate electric field in the radial direction
    G4ThreeVector fieldVector = fieldStrength * position.unit();

    // Set the electric field components
    Efield[0] = fieldVector.x();
    Efield[1] = fieldVector.y();
    Efield[2] = fieldVector.z();

G4cout << "Field at position (" << Point[0] << ", " << Point[1] << ", " << Point[2] << "): "
<< Efield[0] << ", " << Efield[1] << ", " << Efield[2] << G4endl;

}

// Destructor (if any cleanup is required, though not needed here)
G4RadialElectricField::~G4RadialElectricField() = default;
