// SumRadialFieldMap.cc
#include "SumRadialFieldMap.hh"
#include <cmath>
#include <stdexcept>

#include <fstream>    
#include <iomanip>    // (optional, for precision)
#include <omp.h>  // Add at the top

SumRadialFieldMap::SumRadialFieldMap(const std::vector<G4ThreeVector>& pos,
                                     const std::vector<G4double>& charges,
                                     const G4ThreeVector& min,
                                     const G4ThreeVector& max,
                                     const G4ThreeVector& step,
                                     const std::string& filename,
                                     StorageType storage)
: fPositions(pos),
  fCharges(charges),
  fMin(min),
  fMax(max),
  fStep(step),
  fStorage(storage)
{
  // Basic validation
  if (pos.size() != charges.size()) {
    throw std::runtime_error("SumRadialFieldMap: positions and charges size mismatch.");
  }
  if (fStep.x() <= 0 || fStep.y() <= 0 || fStep.z() <= 0) {
    throw std::runtime_error("SumRadialFieldMap: step must be positive in all axes.");
  }
  if (fMax.x() <= fMin.x() || fMax.y() <= fMin.y() || fMax.z() <= fMin.z()) {
    throw std::runtime_error("SumRadialFieldMap: max must exceed min in all axes.");
  }

  // Compute grid dimensions (inclusive endpoints)
  fNx = static_cast<int>(std::floor((fMax.x() - fMin.x()) / fStep.x())) + 1;
  fNy = static_cast<int>(std::floor((fMax.y() - fMin.y()) / fStep.y())) + 1;
  fNz = static_cast<int>(std::floor((fMax.z() - fMin.z()) / fStep.z())) + 1;

  // Normalize fMax to align exactly with last node to avoid half-cell gaps
  fMax = G4ThreeVector(fMin.x() + (fNx - 1) * fStep.x(),
                       fMin.y() + (fNy - 1) * fStep.y(),
                       fMin.z() + (fNz - 1) * fStep.z());

  // Allocate storage
  const size_t total = static_cast<size_t>(fNx) * fNy * fNz;
  if (fStorage == StorageType::Double) {
    fGridD.resize(total);
  } else {
    fGridF.resize(total);
  }

  // Build the map
  BuildFieldMap();

  ExportFieldMapToFile(filename);
}
void SumRadialFieldMap::BuildFieldMap() {
  const G4double coulombsConstant = 1.0 / (4.0 * pi * (1.0 / (mu0 * c_light * c_light)));
 
  // --- Pre-calculate scaled charges to avoid redundant calculations inside the loop ---
  std::vector<G4double> scaledCharges(fCharges.size());
  for (size_t i = 0; i < fCharges.size(); ++i) {
    scaledCharges[i] = fCharges[i] * coulombsConstant;
  }
 
  // --- OPTIMIZED PARALLEL LOOP ---
  // 1. Use ONLY ONE pragma for all three loops.
  // 2. Use collapse(3) to merge the three loops into one large parallel task.
  // 3. Use schedule(static) for this uniform workload, as it has less overhead.
  #pragma omp parallel for schedule(static) collapse(3)
  for (int ix = 0; ix < fNx; ++ix) {
    for (int iy = 0; iy < fNy; ++iy) {
      for (int iz = 0; iz < fNz; ++iz) {
        // These calculations are now distributed efficiently across all threads
        const G4double x = fMin.x() + ix * fStep.x();
        const G4double y = fMin.y() + iy * fStep.y();
        const G4double z = fMin.z() + iz * fStep.z();
 
        G4ThreeVector r(x, y, z);
        G4ThreeVector E(0., 0., 0.);
 
        // Coulomb sum over all 1,000,000 charges
        for (size_t i = 0; i < fPositions.size(); ++i) {
          G4ThreeVector dr = r - fPositions[i];
          G4double d2 = dr.mag2(); // Use squared magnitude (faster, no sqrt)
          if (d2 > 0.) {
            G4double invd = 1.0 / std::sqrt(d2); // Only one sqrt now
            G4double invr3 = invd * invd * invd;
            E += scaledCharges[i] * (dr * invr3); // Use pre-calculated value
          }
        }
 
        const int id = Index(ix, iy, iz);
        if (fStorage == StorageType::Double) {
          fGridD[id] = E;
        }
        else {
          fGridF[id] = Vec3f{ static_cast<float>(E.x()),
                                static_cast<float>(E.y()),
                                static_cast<float>(E.z()) };
        }
      }
    }
  }
}

G4ThreeVector SumRadialFieldMap::Interpolate(int ix, int iy, int iz,
                                             double fx, double fy, double fz) const
{
  // Fetch 8 corners
  const G4ThreeVector c000 = Load(ix,   iy,   iz);
  const G4ThreeVector c100 = Load(ix+1, iy,   iz);
  const G4ThreeVector c010 = Load(ix,   iy+1, iz);
  const G4ThreeVector c110 = Load(ix+1, iy+1, iz);
  const G4ThreeVector c001 = Load(ix,   iy,   iz+1);
  const G4ThreeVector c101 = Load(ix+1, iy,   iz+1);
  const G4ThreeVector c011 = Load(ix,   iy+1, iz+1);
  const G4ThreeVector c111 = Load(ix+1, iy+1, iz+1);

  // Trilinear interpolation
  const double wx0 = 1.0 - fx, wx1 = fx;
  const double wy0 = 1.0 - fy, wy1 = fy;
  const double wz0 = 1.0 - fz, wz1 = fz;

  return
    c000 * (wx0 * wy0 * wz0) +
    c100 * (wx1 * wy0 * wz0) +
    c010 * (wx0 * wy1 * wz0) +
    c110 * (wx1 * wy1 * wz0) +
    c001 * (wx0 * wy0 * wz1) +
    c101 * (wx1 * wy0 * wz1) +
    c011 * (wx0 * wy1 * wz1) +
    c111 * (wx1 * wy1 * wz1);
}

void SumRadialFieldMap::GetFieldValue(const G4double point[4], G4double Field[6]) const {
  // Position from query
  const G4ThreeVector r(point[0], point[1], point[2]);

  // Map to cell indices
  int ix = static_cast<int>(std::floor((r.x() - fMin.x()) / fStep.x()));
  int iy = static_cast<int>(std::floor((r.y() - fMin.y()) / fStep.y()));
  int iz = static_cast<int>(std::floor((r.z() - fMin.z()) / fStep.z()));

  // Clamp to valid range for interpolation (need ix..ix+1 etc.)
  ix = std::max(0, std::min(ix, fNx - 2));
  iy = std::max(0, std::min(iy, fNy - 2));
  iz = std::max(0, std::min(iz, fNz - 2));

  // Fractional coordinates inside the cell
  const double x0 = fMin.x() + ix * fStep.x();
  const double y0 = fMin.y() + iy * fStep.y();
  const double z0 = fMin.z() + iz * fStep.z();

  const double fx = std::clamp((r.x() - x0) / fStep.x(), 0.0, 1.0);
  const double fy = std::clamp((r.y() - y0) / fStep.y(), 0.0, 1.0);
  const double fz = std::clamp((r.z() - z0) / fStep.z(), 0.0, 1.0);

  // Interpolate field
  const G4ThreeVector E = Interpolate(ix, iy, iz, fx, fy, fz);

  // Return as electric field; no magnetic component
  Field[0] = Field[1] = Field[2] = 0.0;
  Field[3] = E.x();
  Field[4] = E.y();
  Field[5] = E.z();
}

void SumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        G4Exception("SumRadialFieldMap::ExportFieldMapToFile",
                    "FileOpenError", FatalException,
                    ("Failed to open file: " + filename).c_str());
        return;
    }

    outfile << "# x y z Ex Ey Ez\n";
    outfile << std::scientific << std::setprecision(6);

    for (int ix = 0; ix < fNx; ++ix) {
        const G4double x = fMin.x() + ix * fStep.x();
        for (int iy = 0; iy < fNy; ++iy) {
            const G4double y = fMin.y() + iy * fStep.y();
            for (int iz = 0; iz < fNz; ++iz) {
                const G4double z = fMin.z() + iz * fStep.z();

                const int id = Index(ix, iy, iz);
                G4ThreeVector E;
                if (fStorage == StorageType::Double) {
                    E = fGridD[id];
                } else {
                    const Vec3f& Ef = fGridF[id];
                    E = G4ThreeVector(Ef.x, Ef.y, Ef.z);
                }

                outfile << x << " " << y << " " << z << " "
                        << E.x() << " " << E.y() << " " << E.z() << "\n";
            }
        }
    }

    outfile.close();
    G4cout << "Field map exported to: " << filename << G4endl;
}
