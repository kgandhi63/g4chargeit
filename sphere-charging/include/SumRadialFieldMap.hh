// SumRadialFieldMap.hh
#ifndef SumRadialFieldMap_h
#define SumRadialFieldMap_h

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <vector>
#include <algorithm>
#include <string>  // Required for std::string

class SumRadialFieldMap : public G4ElectricField {
public:
  // StorageType chooses precision vs memory: double (24 B/point) or float (~12 B/point)
  enum class StorageType { Double, Float };

  // Construct with charge positions and values, plus grid definition
  SumRadialFieldMap(const std::vector<G4ThreeVector>& pos,
                    const std::vector<G4double>& charges,
                    const G4ThreeVector& min,
                    const G4ThreeVector& max,
                    const G4ThreeVector& step,
                    StorageType storage = StorageType::Double);

  ~SumRadialFieldMap() override = default;

  void GetFieldValue(const G4double point[4], G4double Field[6]) const override;

  void ExportFieldMapToFile(const std::string& filename) const;

private:
  // Input charges
  std::vector<G4ThreeVector> fPositions;
  std::vector<G4double>      fCharges;

  // Grid definition
  G4ThreeVector fMin;
  G4ThreeVector fMax;
  G4ThreeVector fStep;
  int fNx = 0, fNy = 0, fNz = 0;

  // Storage choice
  StorageType fStorage = StorageType::Double;

  // Field grid (flattened 3D). Only one is used based on fStorage.
  std::vector<G4ThreeVector> fGridD;     // double-precision storage
  struct Vec3f { float x,y,z; };
  std::vector<Vec3f>         fGridF;     // float-precision storage

  // Helpers
  inline int Index(int ix, int iy, int iz) const {
    return ix + fNx * (iy + fNy * iz);
  }

  inline G4ThreeVector Load(int ix, int iy, int iz) const {
    const int id = Index(ix,iy,iz);
    if (fStorage == StorageType::Double) {
      return fGridD[id];
    } else {
      const auto &v = fGridF[id];
      return G4ThreeVector(v.x, v.y, v.z);
    }
  }

  G4ThreeVector Interpolate(int ix, int iy, int iz,
                            double fx, double fy, double fz) const;

  void BuildFieldMap();
};

#endif // SumRadialFieldMap_h