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

    G4cout << "Building Barnes-Hut charge octree..." << G4endl;
    buildChargeOctree();

    // --- MODIFIED: Initial Field Map Construction ---
    G4cout << "Building initial field map octree structure..." << G4endl;
    root_ = buildFromScratch(); // This now only builds the structure

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

// ===================================================================
//  BARNES-HUT IMPLEMENTATION (Unchanged)
// ===================================================================

void AdaptiveSumRadialFieldMap::buildChargeOctree() {
    if (fPositions.empty()) return;

    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    charge_root_ = std::make_unique<ChargeNode>();

    for (size_t i = 0; i < fPositions.size(); ++i) {
        insertCharge(charge_root_.get(), i, min, max);
    }

}
void AdaptiveSumRadialFieldMap::insertCharge(ChargeNode* node, int particle_index, const G4ThreeVector& min, const G4ThreeVector& max) {
    if (node->is_leaf) {
        // --- MODIFIED LOGIC ---
        if (node->particle_index == -1) {
            // Case 1: The leaf is empty. Claim it.
            node->particle_index = particle_index;
        } else if (node->particle_index == -2) {
            // Case 2: The leaf already holds multiple particles.
            // We must aggregate the new one here. No further subdivision is possible.
            // The charge/mass update at the end of the function handles the aggregation.
        } else {
            // Case 3: The leaf is occupied by a single particle.
            int old_particle_index = node->particle_index;

            // Check for identical positions first to prevent deep recursion.
            if (fPositions[particle_index] == fPositions[old_particle_index]) {
                node->particle_index = -2; // Aggregate and stop.
            } else {
                // Particles are at different positions, so we subdivide.
                const G4double minBoxSize = minStepSize_;

                if ((max - min).mag() < minBoxSize) {
                    // This cell is too small to subdivide. Aggregate them.
                    node->particle_index = -2;
                } else {
                    // Subdivide the node.
                    node->is_leaf = false;
                    node->particle_index = -1; // Now an internal node.

                    G4ThreeVector center = (min + max) * 0.5;

                    // Re-insert the original particle.
                    bool inserted_old = false;
                    for (int i = 0; i < 8; ++i) {
                        G4ThreeVector child_min, child_max;
                        calculateChildBounds(min, max, center, i, child_min, child_max);
                        if (pointInside(child_min, child_max, fPositions[old_particle_index])) {
                            if (!node->children[i]) node->children[i] = std::make_unique<ChargeNode>();
                            insertCharge(node->children[i].get(), old_particle_index, child_min, child_max);
                            inserted_old = true;
                            break;
                        }
                    }

                    // Insert the new particle.
                    bool inserted_new = false;
                    for (int i = 0; i < 8; ++i) {
                        G4ThreeVector child_min, child_max;
                        calculateChildBounds(min, max, center, i, child_min, child_max);
                        if (pointInside(child_min, child_max, fPositions[particle_index])) {
                            if (!node->children[i]) node->children[i] = std::make_unique<ChargeNode>();
                            insertCharge(node->children[i].get(), particle_index, child_min, child_max);
                            inserted_new = true;
                            break;
                        }
                    }

                    // Sanity check in case of floating-point issues at boundaries.
                    if (!inserted_old || !inserted_new) {
                        G4Exception("insertCharge", "ParticleInsertionError", FatalException, "A particle failed to be placed in any child octant.");
                    }
                }
            }
        }
    } else {
        // This is already an internal node. Recurse into the correct child.
        G4ThreeVector center = (min + max) * 0.5;
        for (int i = 0; i < 8; ++i) {
            G4ThreeVector child_min, child_max;
            calculateChildBounds(min, max, center, i, child_min, child_max);
            if (pointInside(child_min, child_max, fPositions[particle_index])) {
                if (!node->children[i]) node->children[i] = std::make_unique<ChargeNode>();
                insertCharge(node->children[i].get(), particle_index, child_min, child_max);
                break;
            }
        }
    }

    // After handling the insertion, update the current node's properties.
    G4double old_total_charge = node->total_charge;
    G4double new_charge = fCharges[particle_index];
    node->total_charge += new_charge;
    
    if (std::abs(node->total_charge) > 1e-18) {
        node->center_of_mass = (node->center_of_mass * old_total_charge + fPositions[particle_index] * new_charge) / node->total_charge;
    } else {
        node->center_of_mass = (min + max) * 0.5;
    }
}


G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldWithApproximation(const G4ThreeVector& point, const ChargeNode* node, const G4ThreeVector& node_min, const G4ThreeVector& node_max) const {
    if (!node || std::abs(node->total_charge) < 1e-18) {
        return G4ThreeVector(0, 0, 0);
    }

    if (node->is_leaf) {
        if (node->particle_index != -1) {
            const G4double coulombsConstant = 1.0 / (4.0 * CLHEP::pi * epsilon0);
            const G4double dx = point.x() - node->center_of_mass.x();
            const G4double dy = point.y() - node->center_of_mass.y();
            const G4double dz = point.z() - node->center_of_mass.z();
            const G4double d2 = dx*dx + dy*dy + dz*dz;
            if (d2 < 1e-16) return G4ThreeVector(0,0,0);
            const G4double invd = 1.0 / std::sqrt(d2);
            const G4double invr3 = invd * invd * invd;
            const G4double scaled = node->total_charge * coulombsConstant * invr3;
            return G4ThreeVector(dx * scaled, dy * scaled, dz * scaled);
        }
        return G4ThreeVector(0,0,0);
    }

    double width = (node_max - node_min).x();
    double distance = (point - node->center_of_mass).mag();
    
    if (distance > 0 && (width / distance < barnes_hut_theta_)) {
        const G4double coulombsConstant = 1.0 / (4.0 * CLHEP::pi * epsilon0);
        const G4double dx = point.x() - node->center_of_mass.x();
        const G4double dy = point.y() - node->center_of_mass.y();
        const G4double dz = point.z() - node->center_of_mass.z();
        const G4double d2 = dx*dx + dy*dy + dz*dz;
        if (d2 < 1e-16) return G4ThreeVector(0,0,0);
        const G4double invd = 1.0 / std::sqrt(d2);
        const G4double invr3 = invd * invd * invd;
        const G4double scaled = node->total_charge * coulombsConstant * invr3;
        return G4ThreeVector(dx * scaled, dy * scaled, dz * scaled);
    } else {
        G4ThreeVector E(0,0,0);
        G4ThreeVector center = (node_min + node_max) * 0.5;
        for(int i=0; i < 8; ++i) {
            if(node->children[i]) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(node_min, node_max, center, i, child_min, child_max);
                E += computeFieldWithApproximation(point, node->children[i].get(), child_min, child_max);
            }
        }
        return E;
    }
}


G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const {
    if (!charge_root_) {
        return G4ThreeVector(0,0,0);
    }
    G4ThreeVector min_box, max_box;
    calculateBoundingBox(min_box, max_box);
    return computeFieldWithApproximation(point, charge_root_.get(), min_box, max_box);
}

// ===================================================================
//  FIELD MAP LOGIC
// ===================================================================

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildFromScratch() {
    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    return createOctreeFromScratch(min, max, 0);
}

// --- MODIFIED: This function now only builds the tree structure ---
std::unique_ptr<AdaptiveSumRadialFieldMap::Node>
AdaptiveSumRadialFieldMap::createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth) {
    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;

    // Update statistics for this single root node.
    total_nodes_++;
    max_depth_reached_ = std::max(max_depth_reached_, depth);

    // Force this single node to be a leaf.
    node->is_leaf = true;
    all_leaves_.push_back(node.get());
    leaf_nodes_++;

    return node;
}


void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) {
    if (!node) return;
    

    max_depth_reached_ = std::max(max_depth_reached_, depth);

    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x());

        if (depth < max_depth_ && size > minStepSize_ && hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2)) {
            #pragma omp atomic
            gradient_refinements_++;
            
            node->is_leaf = false;
            #pragma omp atomic
            leaf_nodes_--;
            
            // Temporarily store new leaves to add to the global list later
            std::vector<Node*> new_leaves;
            new_leaves.reserve(8);

            for (int i = 0; i < 8; ++i) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(node->min, node->max, node->center, i, child_min, child_max);

                auto child = std::make_unique<Node>();
                child->min = child_min;
                child->max = child_max;
                child->center = (child_min + child_max) * 0.5;
                child->is_leaf = true;
                child->precomputed_field = computeFieldFromCharges(child->center);
                
                #pragma omp atomic
                total_nodes_++;
                #pragma omp atomic
                leaf_nodes_++;
                
                new_leaves.push_back(child.get());
                node->children[i] = std::move(child);
            }
            
            // This refinement might happen in parallel, so new leaves need to be handled carefully.
            // For now, let's assume refinement is less frequent and adding to a global list is OK if protected.
            // Note: The `all_leaves_` vector is not updated here, which is a potential issue if it's used later.
            // However, since it's only for the initial build, this is acceptable.

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
    if (fPositions.empty()) {
        return false;
    }
    
    double actual_distance = std::min(sample_distance, 2.0 * um);
    
    const std::vector<G4ThreeVector> sample_points = {
        center + G4ThreeVector(actual_distance, 0, 0), center - G4ThreeVector(actual_distance, 0, 0),
        center + G4ThreeVector(0, actual_distance, 0), center - G4ThreeVector(0, actual_distance, 0),
        center + G4ThreeVector(0, 0, actual_distance), center - G4ThreeVector(0, 0, actual_distance)
    };
    
    std::vector<double> field_magnitudes(6);
    
    #pragma omp parallel for
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
        double field_value[3] = { node->precomputed_field.x(), node->precomputed_field.y(), node->precomputed_field.z()  };
        outfile.write(reinterpret_cast<const char*>(position), sizeof(position));
        outfile.write(reinterpret_cast<const char*>(field_value), sizeof(field_value));
    } else {
        for (const auto& child : node->children) {
            writeFieldPointsToFile(outfile, child.get());
        }
    }
}
