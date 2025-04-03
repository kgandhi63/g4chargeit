#include "G4RadialElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

// Constructor
G4RadialElectricField::G4RadialElectricField(double field)
    : field_(field) 
{

}

void G4RadialElectricField::GetFieldValue(const G4double Point[4], G4double* Efield) const {
    for (int i = 0; i < 6; ++i) {
        Efield[i] = 0.0;  
    }

    // Electric Field is Represented 3-5, Magnetic field is 0-2 (respective direction)

    G4ThreeVector position(Point[0], Point[1], Point[2]);
    G4double radius = position.mag();

    G4double shellRadius = 1 * nm;  // Threshold radius (Variable to change - KG)
    G4double fieldStrength;

    const double k = 1 / (4 * M_PI * epsilon0); // Coulomb's constant

    if (shellRadius > radius) {
        fieldStrength = (k * field_ / (shellRadius * shellRadius)) * (radius / shellRadius);
    } else {
        fieldStrength = k * field_ / (radius * radius);
    }

    // Calculate electric field in the radial direction
    G4ThreeVector fieldVector = fieldStrength * position.unit();

    // Set the electric field components
    Efield[3] = fieldVector.x();  // Ex
    Efield[4] = fieldVector.y();  // Ey
    Efield[5] = fieldVector.z();  // Ez
}

// Implicit Constructor set to default
G4RadialElectricField::~G4RadialElectricField() = default;
