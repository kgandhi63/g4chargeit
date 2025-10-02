// SumRadialFieldMap.cc
#include "SumRadialFieldMap.hh"
#include <cmath>
#include <stdexcept>

#include <immintrin.h> // For AVX intrinsics
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

  // --- Enhanced precomputation ---
  std::vector<G4double> scaledCharges(fCharges.size());
  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < fCharges.size(); ++i) {
    scaledCharges[i] = fCharges[i] * coulombsConstant;
  }

  // Convert to Structure of Arrays (SoA) for better vectorization
  struct ChargeData {
    std::vector<G4double> x, y, z, charge;
  };
  
  ChargeData chargeData;
  chargeData.x.resize(fPositions.size());
  chargeData.y.resize(fPositions.size());
  chargeData.z.resize(fPositions.size());
  chargeData.charge.resize(fPositions.size());
  
  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < fPositions.size(); ++i) {
    chargeData.x[i] = fPositions[i].x();
    chargeData.y[i] = fPositions[i].y();
    chargeData.z[i] = fPositions[i].z();
    chargeData.charge[i] = scaledCharges[i];
  }

  // Precompute constants
  const G4double min_x = fMin.x();
  const G4double min_y = fMin.y();
  const G4double min_z = fMin.z();
  const G4double step_x = fStep.x();
  const G4double step_y = fStep.y();
  const G4double step_z = fStep.z();
  const size_t num_charges = fPositions.size();
  const G4double epsilon = 1e-16; // Small value to avoid division by zero

  // --- OPTIMIZED PARALLEL LOOP WITH VECTORIZATION ---
  #pragma omp parallel for schedule(static) collapse(2)
  for (int ix = 0; ix < fNx; ++ix) {
    for (int iy = 0; iy < fNy; ++iy) {
      
      // Precompute x,y for this 2D slice
      const G4double x = min_x + ix * step_x;
      const G4double y = min_y + iy * step_y;
      
      // Precompute differences in x,y for all charges
      std::vector<G4double> dx(num_charges), dy(num_charges);
      for (size_t i = 0; i < num_charges; ++i) {
        dx[i] = x - chargeData.x[i];
        dy[i] = y - chargeData.y[i];
      }
      
      for (int iz = 0; iz < fNz; ++iz) {
        const G4double z = min_z + iz * step_z;
        
        G4double Ex = 0.0, Ey = 0.0, Ez = 0.0;

        // Vectorized inner loop (manual unrolling + SIMD-friendly)
        size_t i = 0;
        
        // Use manual unrolling for better pipelining
        for (; i + 3 < num_charges; i += 4) {
          for (int j = 0; j < 4; ++j) {
            const size_t idx = i + j;
            const G4double dz = z - chargeData.z[idx];
            const G4double dx_val = dx[idx];
            const G4double dy_val = dy[idx];
            
            const G4double d2 = dx_val*dx_val + dy_val*dy_val + dz*dz;
            
            if (d2 > epsilon) {
              const G4double invd = 1.0 / std::sqrt(d2);
              const G4double invr3 = invd * invd * invd;
              const G4double scaled = chargeData.charge[idx] * invr3;
              
              Ex += dx_val * scaled;
              Ey += dy_val * scaled;
              Ez += dz * scaled;
            }
          }
        }
        
        // Remainder loop
        for (; i < num_charges; ++i) {
          const G4double dz = z - chargeData.z[i];
          const G4double dx_val = dx[i];
          const G4double dy_val = dy[i];
          
          const G4double d2 = dx_val*dx_val + dy_val*dy_val + dz*dz;
          
          if (d2 > epsilon) {
            const G4double invd = 1.0 / std::sqrt(d2);
            const G4double invr3 = invd * invd * invd;
            const G4double scaled = chargeData.charge[i] * invr3;
            
            Ex += dx_val * scaled;
            Ey += dy_val * scaled;
            Ez += dz * scaled;
          }
        }

        const int id = Index(ix, iy, iz);
        if (fStorage == StorageType::Double) {
          fGridD[id] = G4ThreeVector(Ex, Ey, Ez);
        } else {
          fGridF[id] = Vec3f{ static_cast<float>(Ex),
                              static_cast<float>(Ey),
                              static_cast<float>(Ez) };
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
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        G4Exception("SumRadialFieldMap::ExportFieldMapToFileBinary",
                    "FileOpenError", FatalException,
                    ("Failed to open file: " + filename).c_str());
        return;
    }
    
    // Write header with metadata
    int dimensions[3] = {fNx, fNy, fNz};
    double min[3] = {fMin.x(), fMin.y(), fMin.z()};
    double step[3] = {fStep.x(), fStep.y(), fStep.z()};
    int storage_type = static_cast<int>(fStorage);
    
    outfile.write(reinterpret_cast<const char*>(dimensions), sizeof(dimensions));
    outfile.write(reinterpret_cast<const char*>(min), sizeof(min));
    outfile.write(reinterpret_cast<const char*>(step), sizeof(step));
    outfile.write(reinterpret_cast<const char*>(&storage_type), sizeof(storage_type));
    
    // Write field data
    if (fStorage == StorageType::Double) {
        outfile.write(reinterpret_cast<const char*>(fGridD.data()), 
                     fGridD.size() * sizeof(G4ThreeVector));
    } else {
        outfile.write(reinterpret_cast<const char*>(fGridF.data()), 
                     fGridF.size() * sizeof(Vec3f));
    }
    
    outfile.close();
    G4cout << "Binary field map exported to: " << filename << G4endl;
}