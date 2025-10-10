#include "AdaptiveSumRadialFieldMap.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Exception.hh"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <stdexcept>
#include <iomanip>
#include <vector>

AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    const std::vector<G4ThreeVector>& positions,
    const std::vector<G4double>& charges,
    const G4double& gradThreshold,
    const G4double& minStep,
    const std::string& filename,
    const G4ThreeVector& min,
    const G4ThreeVector& max,
    int max_depth,
    StorageType storage)
    : max_depth_(max_depth), minStepSize_(minStep),
      worldMin_(min), worldMax_(max), fieldGradThreshold_(gradThreshold), fStorage(storage),
      fPositions(positions), fCharges(charges)
{
    total_nodes_ = 0;
    leaf_nodes_ = 0;
    gradient_refinements_ = 0;
    max_depth_reached_ = 0;

    setWorldDimensions(worldMax_.x() - worldMin_.x(), worldMax_.y() - worldMin_.y(), worldMax_.z() - worldMin_.z());

    // --- PARTICLE-IN-CELL (PIC) WORKFLOW ---
    
    // The resolution of the PIC grid is a key performance parameter.
    // 64^3 is a good balance of speed and accuracy for the solver.
    int pic_grid_resolution = 64;
    G4cout << "Initializing PIC grid (" << pic_grid_resolution << "^3)..." << G4endl;
    G4cout << "max depth input: " << max_depth_ << G4endl;
    initializePICGrid(pic_grid_resolution, pic_grid_resolution, pic_grid_resolution);

    G4cout << "Depositing charges onto PIC grid..." << G4endl;
    depositCharges();

    // This is now the FFT-based solver, which is O(N log N) and much faster.
    G4cout << "Solving for electric field on PIC grid using FFT..." << G4endl;
    solveFieldOnGrid();

    // --- END PIC WORKFLOW ---

    G4cout << "Building initial field map octree structure..." << G4endl;
    root_ = buildFromScratch(); 

    G4cout << "Computing field values for " << all_leaves_.size() << " leaf nodes in parallel..." << G4endl;
    #pragma omp parallel for schedule(dynamic)
    for (Node* leaf : all_leaves_) {
        leaf->precomputed_field = computeFieldFromCharges(leaf->center);
    }
    
    G4cout << "Refining field map based on field gradients..." << G4endl;
    #pragma omp parallel
    {
        #pragma omp single
        refineMeshByGradient(root_.get(), 0);
    }

    collectStatistics(root_.get(), 0);
    if (!filename.empty()) {
        ExportFieldMapToFile(filename);
    }
    PrintMeshStatistics();
}

// --- NEW: Destructor to clean up FFTW memory ---
AdaptiveSumRadialFieldMap::~AdaptiveSumRadialFieldMap() {
    fftw_destroy_plan(fft_plan_forward_);
    fftw_destroy_plan(fft_plan_backward_);
    fftw_free(fft_charge_grid_);
    fftw_free(fft_potential_grid_);
}

// ===================================================================
//  PIC IMPLEMENTATION (with FFT solver)
// ===================================================================

void AdaptiveSumRadialFieldMap::initializePICGrid(int nx, int ny, int nz) {
    fNx_ = nx;
    fNy_ = ny;
    fNz_ = nz;
    
    fStep_ = G4ThreeVector((fNx_ > 1) ? (worldMax_.x() - worldMin_.x()) / (fNx_ - 1) : 0.0,
                           (fNy_ > 1) ? (worldMax_.y() - worldMin_.y()) / (fNy_ - 1) : 0.0,
                           (fNz_ > 1) ? (worldMax_.z() - worldMin_.z()) / (fNz_ - 1) : 0.0);

    size_t gridSize = static_cast<size_t>(fNx_) * fNy_ * fNz_;
    charge_grid_.assign(gridSize, 0.0);
    potential_grid_.assign(gridSize, 0.0);
    field_grid_.assign(gridSize, G4ThreeVector(0,0,0));

    // --- Initialize FFTW ---
    fft_charge_grid_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * gridSize);
    fft_potential_grid_ = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * gridSize);

    // Create plans for the 3D FFT. This is done once and reused.
    fft_plan_forward_ = fftw_plan_dft_3d(fNz_, fNy_, fNx_, fft_charge_grid_, fft_charge_grid_, FFTW_FORWARD, FFTW_ESTIMATE);
    fft_plan_backward_ = fftw_plan_dft_3d(fNz_, fNy_, fNx_, fft_potential_grid_, fft_potential_grid_, FFTW_BACKWARD, FFTW_ESTIMATE);
}

void AdaptiveSumRadialFieldMap::depositCharges() {
    if (fPositions.empty()) return;

    #pragma omp parallel for
    for (size_t i = 0; i < fPositions.size(); ++i) {
        const G4ThreeVector& pos = fPositions[i];
        const double charge = fCharges[i];

        double ix_f = (fStep_.x() > 0) ? (pos.x() - worldMin_.x()) / fStep_.x() : 0;
        double iy_f = (fStep_.y() > 0) ? (pos.y() - worldMin_.y()) / fStep_.y() : 0;
        double iz_f = (fStep_.z() > 0) ? (pos.z() - worldMin_.z()) / fStep_.z() : 0;

        int ix = static_cast<int>(ix_f);
        int iy = static_cast<int>(iy_f);
        int iz = static_cast<int>(iz_f);

        double wx = ix_f - ix;
        double wy = iy_f - iy;
        double wz = iz_f - iz;

        for (int l = 0; l < 2; ++l) {
            for (int m = 0; m < 2; ++m) {
                for (int n = 0; n < 2; ++n) {
                    int grid_ix = ix + l;
                    int grid_iy = iy + m;
                    int grid_iz = iz + n;

                    if (grid_ix >= 0 && grid_ix < fNx_ && grid_iy >= 0 && grid_iy < fNy_ && grid_iz >= 0 && grid_iz < fNz_) {
                        double weight = (l == 0 ? 1 - wx : wx) * (m == 0 ? 1 - wy : wy) * (n == 0 ? 1 - wz : wz);
                        size_t index = static_cast<size_t>(grid_iz) * fNx_ * fNy_ + static_cast<size_t>(grid_iy) * fNx_ + grid_ix;
                        
                        #pragma omp atomic
                        charge_grid_[index] += charge * weight;
                    }
                }
            }
        }
    }
}

// --- REWRITTEN: This is now the high-performance FFT-based Poisson Solver ---
void AdaptiveSumRadialFieldMap::solveFieldOnGrid() {
    size_t gridSize = static_cast<size_t>(fNx_) * fNy_ * fNz_;

    // 1. Copy charge density to FFTW input array
    for (size_t i = 0; i < gridSize; ++i) {
        fft_charge_grid_[i][0] = charge_grid_[i]; // Real part
        fft_charge_grid_[i][1] = 0.0;            // Imaginary part
    }

    // 2. Execute forward FFT (charge density -> Fourier space)
    fftw_execute(fft_plan_forward_);

    // 3. Apply the Green's function (1/k^2) in Fourier space
    const double kx_factor = (worldX_ > 0) ? 2.0 * CLHEP::pi / worldX_ : 0;
    const double ky_factor = (worldY_ > 0) ? 2.0 * CLHEP::pi / worldY_ : 0;
    const double kz_factor = (worldZ_ > 0) ? 2.0 * CLHEP::pi / worldZ_ : 0;

    #pragma omp parallel for collapse(3)
    for (int iz = 0; iz < fNz_; ++iz) {
        for (int iy = 0; iy < fNy_; ++iy) {
            for (int ix = 0; ix < fNx_; ++ix) {
                // FFTW shifts the frequencies, so we need to map them back
                int kx = (ix > fNx_ / 2) ? ix - fNx_ : ix;
                int ky = (iy > fNy_ / 2) ? iy - fNy_ : iy;
                int kz = (iz > fNz_ / 2) ? iz - fNz_ : iz;

                double k2 = (kx*kx_factor)*(kx*kx_factor) + 
                              (ky*ky_factor)*(ky*ky_factor) + 
                              (kz*kz_factor)*(kz*kz_factor);

                size_t index = static_cast<size_t>(iz) * fNx_ * fNy_ + static_cast<size_t>(iy) * fNx_ + ix;

                if (k2 > 1e-12) { // Avoid division by zero for the DC component (k=0)
                    fft_potential_grid_[index][0] = fft_charge_grid_[index][0] / (k2 * epsilon0);
                    fft_potential_grid_[index][1] = fft_charge_grid_[index][1] / (k2 * epsilon0);
                } else {
                    fft_potential_grid_[index][0] = 0.0;
                    fft_potential_grid_[index][1] = 0.0;
                }
            }
        }
    }

    // 4. Execute backward FFT (Fourier space -> electric potential)
    fftw_execute(fft_plan_backward_);

    // 5. Copy potential back and normalize
    for (size_t i = 0; i < gridSize; ++i) {
        potential_grid_[i] = fft_potential_grid_[i][0] / gridSize;
    }

    // 6. Calculate E-field by taking the gradient of the potential (E = -∇φ)
    #pragma omp parallel for collapse(3)
    for (int iz = 0; iz < fNz_; ++iz) {
        for (int iy = 0; iy < fNy_; ++iy) {
            for (int ix = 0; ix < fNx_; ++ix) {
                size_t idx = static_cast<size_t>(iz) * fNx_ * fNy_ + static_cast<size_t>(iy) * fNx_ + ix;
                double Ex = 0, Ey = 0, Ez = 0;

                // X-component using central differences
                if (ix > 0 && ix < fNx_ - 1) {
                    Ex = -(potential_grid_[idx + 1] - potential_grid_[idx - 1]) / (2 * fStep_.x());
                } // Note: Boundary conditions are implicitly handled by setting E to 0 here.

                // Y-component
                if (iy > 0 && iy < fNy_ - 1) {
                    Ey = -(potential_grid_[idx + fNx_] - potential_grid_[idx - fNx_]) / (2 * fStep_.y());
                }

                // Z-component
                 if (iz > 0 && iz < fNz_ - 1) {
                    Ez = -(potential_grid_[idx + fNx_ * fNy_] - potential_grid_[idx - fNx_ * fNy_]) / (2 * fStep_.z());
                }
                
                field_grid_[idx] = G4ThreeVector(Ex, Ey, Ez);
            }
        }
    }
}

G4ThreeVector AdaptiveSumRadialFieldMap::interpolateFieldFromGrid(const G4ThreeVector& point) const {
    double ix_f = (fStep_.x() > 0) ? (point.x() - worldMin_.x()) / fStep_.x() : 0;
    double iy_f = (fStep_.y() > 0) ? (point.y() - worldMin_.y()) / fStep_.y() : 0;
    double iz_f = (fStep_.z() > 0) ? (point.z() - worldMin_.z()) / fStep_.z() : 0;

    int ix = static_cast<int>(ix_f);
    int iy = static_cast<int>(iy_f);
    int iz = static_cast<int>(iz_f);

    ix = std::max(0, std::min(ix, fNx_ - 2));
    iy = std::max(0, std::min(iy, fNy_ - 2));
    iz = std::max(0, std::min(iz, fNz_ - 2));

    double wx = ix_f - ix;
    double wy = iy_f - iy;
    double wz = iz_f - iz;

    G4ThreeVector result(0,0,0);
    for (int l = 0; l < 2; ++l) {
        for (int m = 0; m < 2; ++m) {
            for (int n = 0; n < 2; ++n) {
                size_t index = static_cast<size_t>(iz+n) * fNx_ * fNy_ + static_cast<size_t>(iy+m) * fNx_ + (ix+l);
                double weight = (l == 0 ? 1 - wx : wx) * (m == 0 ? 1 - wy : wy) * (n == 0 ? 1 - wz : wz);
                result += field_grid_[index] * weight;
            }
        }
    }
    return result;
}

G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const {
    return interpolateFieldFromGrid(point);
}

// ===================================================================
//  ADAPTIVE OCTREE LOGIC (Now using the fast PIC solver)
// ===================================================================

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildFromScratch() {
    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    return createOctreeFromScratch(min, max, 0);
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth) {
    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;

    total_nodes_++;
    max_depth_reached_ = std::max(max_depth_reached_, depth);

    double size = (max.x() - min.x());
    bool should_create_children = (size > 2.0 * minStepSize_) && (depth < max_depth_);

    if (should_create_children) {
        node->is_leaf = false;
        G4ThreeVector center = node->center;
        for (int i = 0; i < 8; ++i) {
            G4ThreeVector child_min, child_max;
            calculateChildBounds(min, max, center, i, child_min, child_max);
            node->children[i] = createOctreeFromScratch(child_min, child_max, depth + 1);
        }
    } else {
        node->is_leaf = true;
        all_leaves_.push_back(node.get());
        leaf_nodes_++;
    }
    return node;
}

void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) {
    if (!node) return;
    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x());
        if (depth < max_depth_ && size > minStepSize_ && hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2)) {
            #pragma omp atomic
            gradient_refinements_++;
            node->is_leaf = false;
            #pragma omp atomic
            leaf_nodes_--;
            for (int i = 0; i < 8; ++i) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(node->min, node->max, node->center, i, child_min, child_max);
                auto child = std::make_unique<Node>();
                child->min = child_min; child->max = child_max; child->center = (child_min + child_max) * 0.5; child->is_leaf = true;
                child->precomputed_field = computeFieldFromCharges(child->center);
                #pragma omp atomic
                total_nodes_++;
                #pragma omp atomic
                leaf_nodes_++;
                node->children[i] = std::move(child);
            }
            for (int i = 0; i < 8; ++i) {
                if(node->children[i]) {
                    #pragma omp task
                    refineMeshByGradient(node->children[i].get(), depth + 1);
                }
            }
            #pragma omp taskwait
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            if(node->children[i]) {
                #pragma omp task
                refineMeshByGradient(node->children[i].get(), depth + 1);
            }
        }
        #pragma omp taskwait
    }
}

bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field, double sample_distance) const {
    if (fPositions.empty()) return false;
    double actual_distance = std::min(sample_distance, 2.0 * um);
    const std::vector<G4ThreeVector> sample_points = {
        center + G4ThreeVector(actual_distance, 0, 0), center - G4ThreeVector(actual_distance, 0, 0),
        center + G4ThreeVector(0, actual_distance, 0), center - G4ThreeVector(0, actual_distance, 0),
        center + G4ThreeVector(0, 0, actual_distance), center - G4ThreeVector(0, 0, actual_distance)
    };
    std::vector<double> field_magnitudes(6);
    // This loop is intentionally sequential to avoid nested parallelism crashes.
    for (int i = 0; i < 6; ++i) {
        field_magnitudes[i] = computeFieldFromCharges(sample_points[i]).mag();
    }
    double grad_x = (field_magnitudes[0] - field_magnitudes[1]) / (2 * actual_distance);
    double grad_y = (field_magnitudes[2] - field_magnitudes[3]) / (2 * actual_distance);
    double grad_z = (field_magnitudes[4] - field_magnitudes[5]) / (2 * actual_distance);
    double gradient_magnitude = std::sqrt(grad_x * grad_x + grad_y * grad_y + grad_z * grad_z);
    return (gradient_magnitude / 1e6) > fieldGradThreshold_;
}

void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const {
    const G4ThreeVector r(point[0], point[1], point[2]);
    G4ThreeVector E = evaluateField(r, root_.get());
    field[0] = field[1] = field[2] = 0.0;
    field[3] = E.x(); field[4] = E.y(); field[5] = E.z();
}

G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point, const Node* node) const {
    if (!node) return G4ThreeVector(0, 0, 0);
    if (node->is_leaf) return node->precomputed_field;
    for (const auto& child : node->children) {
        if (child && pointInside(child->min, child->max, point)) {
            return evaluateField(point, child.get());
        }
    }
    return G4ThreeVector(0, 0, 0);
}

// --- Utility and I/O Functions ---

bool AdaptiveSumRadialFieldMap::pointInside(const G4ThreeVector& min, const G4ThreeVector& max, const G4ThreeVector& point) const {
    return (point.x() >= min.x() && point.x() <= max.x() &&
            point.y() >= min.y() && point.y() <= max.y() &&
            point.z() >= min.z() && point.z() <= max.z());
}

void AdaptiveSumRadialFieldMap::calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max) const {
    min = worldMin_;
    max = worldMax_;
}

void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const {
    G4cout << "=== Adaptive Mesh Statistics ===" << G4endl;
    G4cout << "Total nodes in field map: " << total_nodes_ << G4endl;
    G4cout << "Leaf nodes in field map: " << leaf_nodes_ << G4endl;
    G4cout << "Max depth reached: " << max_depth_reached_ << G4endl;
    G4cout << "Gradient-based refinements: " << gradient_refinements_ << G4endl;
    G4cout << "=================================" << G4endl;
}

void AdaptiveSumRadialFieldMap::collectStatistics(const Node* node, int depth) const {
    if (!node) return;
    if (!node->is_leaf) {
        for (const auto& child : node->children) {
            if (child) collectStatistics(child.get(), depth + 1);
        }
    }
}

void AdaptiveSumRadialFieldMap::calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                                                      const G4ThreeVector& center, int child_index,
                                                      G4ThreeVector& child_min, G4ThreeVector& child_max) const {
    switch (child_index) {
        case 0: child_min = min; child_max = center; break;
        case 1: child_min = G4ThreeVector(center.x(), min.y(), min.z()); child_max = G4ThreeVector(max.x(), center.y(), center.z()); break;
        case 2: child_min = G4ThreeVector(min.x(), center.y(), min.z()); child_max = G4ThreeVector(center.x(), max.y(), center.z()); break;
        case 3: child_min = G4ThreeVector(center.x(), center.y(), min.z()); child_max = G4ThreeVector(max.x(), max.y(), center.z()); break;
        case 4: child_min = G4ThreeVector(min.x(), min.y(), center.z()); child_max = G4ThreeVector(center.x(), center.y(), max.z()); break;
        case 5: child_min = G4ThreeVector(center.x(), min.y(), center.z()); child_max = G4ThreeVector(max.x(), center.y(), max.z()); break;
        case 6: child_min = G4ThreeVector(min.x(), center.y(), center.z()); child_max = G4ThreeVector(center.x(), max.y(), max.z()); break;
        case 7: child_min = center; child_max = max; break;
    }
}
void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, ("Failed to open file: " + filename).c_str());
        return;
    }
    double world_bounds[6] = { worldMin_.x(), worldMin_.y(), worldMin_.z(), worldMax_.x(), worldMax_.y(), worldMax_.z() };
    int mesh_parameters[5] = { max_depth_, static_cast<int>(minStepSize_ / um), static_cast<int>(total_nodes_), static_cast<int>(leaf_nodes_), static_cast<int>(fStorage) };
    outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));
    writeFieldPointsToFile(outfile, root_.get());
    int statistics[3] = { static_cast<int>(gradient_refinements_), max_depth_reached_, static_cast<int>(fPositions.size()) };
    outfile.write(reinterpret_cast<const char*>(statistics), sizeof(statistics));
    outfile.close();
    G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
}

void AdaptiveSumRadialFieldMap::writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const {
    if (!node) return;
    if (node->is_leaf) {
        double position[3] = { node->center.x(), node->center.y(), node->center.z() };
        double field_value[3] = { node->precomputed_field.x(), node->precomputed_field.y() , node->precomputed_field.z()};
        outfile.write(reinterpret_cast<const char*>(position), sizeof(position));
        outfile.write(reinterpret_cast<const char*>(field_value), sizeof(field_value));
    } else {
        for (const auto& child : node->children) {
            writeFieldPointsToFile(outfile, child.get());
        }
    }
}

