#ifndef SumRadialField_h
#define SumRadialField_h

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4PhysicalConstants.hh"   // for permittivityOfVacuum, pi
#include "G4SystemOfUnits.hh"
#include <vector>

class SumRadialField : public G4ElectricField {
public:
  SumRadialField(const std::vector<G4ThreeVector>& pos, G4double q)
    : fPositions(pos), fCharge(q) {}
  
  void GetFieldValue(const G4double point[4], G4double Field[6]) const override {
    G4ThreeVector r(point[0], point[1], point[2]);
    G4ThreeVector E(0., 0., 0.);

    const G4double epsilon0 = 1.0 / (mu0 * c_light * c_light);

    for (auto& ri : fPositions) {
      G4ThreeVector dr = r - ri;
      G4double r3 = std::pow(dr.mag(), 3);
      if (r3 > 0.) {
        E += (fCharge / (4. * pi * epsilon0)) * (dr / r3);
      }
    }

    Field[3] = E.x();
    Field[4] = E.y();
    Field[5] = E.z();
    Field[0] = Field[1] = Field[2] = 0.;
  }

private:
  std::vector<G4ThreeVector> fPositions;
  G4double                   fCharge;
};

#endif  // SumRadialField_h
