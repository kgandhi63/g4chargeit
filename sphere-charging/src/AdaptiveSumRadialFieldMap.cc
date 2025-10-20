#include "AdaptiveSumRadialFieldMap.hh"
#include "G4Exception.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <stdexcept>
#include <iomanip>
#include <vector>

static const G4double k_electric = 1.0 / (4.0 * CLHEP::pi * CLHEP::epsilon0);
// --- MODIFIED CONSTRUCTOR ---
AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    std::vector<G4ThreeVector>& positions,
    std::vector<G4double>& charges,
    const G4double& gradThreshold,
    const G4double& minStep,
    const G4double& time_step_dt,
    const G4double& material_temp_K,
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

    G4cout << "Building initial field map octree structure..." << G4endl;
    root_ = buildFromScratch();

    #pragma omp parallel for schedule(dynamic)
    for (Node* leaf : all_leaves_) {
        leaf->precomputed_field = computeFieldFromCharges(leaf->center);
    }

    // --- ADDED: THE ONE-TIME "TAX" CALCULATION ---
    G4cout << "--> Applying one-time charge dissipation ('tax')..." << G4endl;
    ApplyChargeDissipation(time_step_dt, material_temp_K);
    // The master charge list (fCharges) has now been modified.

    G4cout << "Rebuild Barnes-Hut charge octree with dissipated charge..." << G4endl;
    buildChargeOctree();

    G4cout << "rebuild initial field map octree structure with dissipated charge..." << G4endl;
    root_ = buildFromScratch();

    #pragma omp parallel for schedule(dynamic)
    for (Node* leaf : all_leaves_) {
        leaf->precomputed_field = computeFieldFromCharges(leaf->center);
    }
    
    G4cout << "--> Refining field map based on field gradients..." << G4endl;
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

// --- ADDED: IMPLEMENTATION OF NEW PHYSICS FUNCTIONS ---

void AdaptiveSumRadialFieldMap::ApplyChargeDissipation(G4double dt, G4double temp_K) {
    double conductivity = calculateConductivity(temp_K);

    #pragma omp parallel for schedule(dynamic)
    for (const auto& leaf : all_leaves_) {
            // Convert field to SI units (V/m)
            G4ThreeVector E_field = leaf->precomputed_field;
            // J = σE; Units: (A/m²)
            G4double leakage_current_density = conductivity * E_field.mag();
            // Area in m²
            G4double node_width = (leaf->max.x() - leaf->min.x());
            G4double leaf_area = node_width * node_width; 
            // Q = J · A · t; Units: Coulombs (C)
            G4double charge_to_remove = leakage_current_density * leaf_area * dt;

            if (charge_to_remove > 0) {
                removeChargeFromRegion(leaf->min, leaf->max, charge_to_remove);
                G4cout << "--> amount of charge to remove: "<< charge_to_remove << ", E_field: "<< 
                        G4BestUnit(E_field.mag(), "Electric field") << G4endl;
            }
    }
}

double AdaptiveSumRadialFieldMap::calculateConductivity(double temp_K) const {
    if (temp_K <= 0) return 0.0;
    return 6.0e-18 * std::exp(0.0230 * temp_K);
}

void AdaptiveSumRadialFieldMap::removeChargeFromRegion(const G4ThreeVector& min_bounds, const G4ThreeVector& max_bounds, G4double charge_to_remove) {
    std::vector<int> particle_indices_in_region;

    for (size_t i = 0; i < fPositions.size(); ++i) {
        if (fCharges[i] > 0 && pointInside(min_bounds, max_bounds, fPositions[i])) {
            particle_indices_in_region.push_back(i);
        }
    }

    if (particle_indices_in_region.empty()) return;

    double charge_to_remove_per_particle = charge_to_remove / particle_indices_in_region.size();
    G4cout << "--> charge to remove per particle: "<< charge_to_remove_per_particle << G4endl;

    for (int idx : particle_indices_in_region) {

        #pragma omp atomic
        fCharges[idx] -= charge_to_remove_per_particle;
        if (fCharges[idx] < 1e-21 * eplus) {
            fCharges[idx] = 0;
        }
    }

}



// ===================================================================
//  BARNES-HUT IMPLEMENTATION
// ===================================================================

void AdaptiveSumRadialFieldMap::buildChargeOctree() {
    if (fPositions.empty()) return;

    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    charge_root_ = std::make_unique<ChargeNode>();

    for (size_t i = 0; i < fPositions.size(); ++i) {
        if (std::abs(fCharges[i]) > 1e-25 * CLHEP::coulomb) {
                insertCharge(charge_root_.get(), i, min, max);
        }
    }
}
void AdaptiveSumRadialFieldMap::insertCharge(ChargeNode* node, int particle_index, const G4ThreeVector& min, const G4ThreeVector& max) {
    if (node->is_leaf) {
        if (node->particle_index == -1) {
            // Case 1: The leaf is empty. Claim it.
            node->particle_index = particle_index;
        } else if (node->particle_index == -2) {
            // Case 2: The leaf is already a multi-particle aggregate. Do nothing here;
            // the update at the end of the function handles the aggregation.
        } else {
            // Case 3: The leaf is occupied by a single particle. We must subdivide.
            int old_particle_index = node->particle_index;

            if (fPositions[particle_index] == fPositions[old_particle_index]) {
                node->particle_index = -2; // Aggregate identical particles and stop.
            } else {
                const G4double minBoxSize = 1.0 * nm; // Safety threshold for close particles
                if ((max - min).mag() < minBoxSize) {
                    node->particle_index = -2; // Aggregate if box is too small.
                } else {
                    // --- SUBDIVIDE THE NODE ---
                    node->is_leaf = false;
                    node->particle_index = -1;
                    G4ThreeVector center = (min + max) * 0.5;

                    // --- ROBUST PLACEMENT LOGIC (THE FIX) ---
                    // Directly calculate which child the OLD particle belongs to.
                    int old_child_idx = 0;
                    if (fPositions[old_particle_index].x() >= center.x()) old_child_idx |= 1;
                    if (fPositions[old_particle_index].y() >= center.y()) old_child_idx |= 2;
                    if (fPositions[old_particle_index].z() >= center.z()) old_child_idx |= 4;

                    // Create that child and re-insert the old particle.
                    G4ThreeVector old_child_min, old_child_max;
                    calculateChildBounds(min, max, center, old_child_idx, old_child_min, old_child_max);
                    if (!node->children[old_child_idx]) node->children[old_child_idx] = std::make_unique<ChargeNode>();
                    insertCharge(node->children[old_child_idx].get(), old_particle_index, old_child_min, old_child_max);

                    // Directly calculate which child the NEW particle belongs to.
                    int new_child_idx = 0;
                    if (fPositions[particle_index].x() >= center.x()) new_child_idx |= 1;
                    if (fPositions[particle_index].y() >= center.y()) new_child_idx |= 2;
                    if (fPositions[particle_index].z() >= center.z()) new_child_idx |= 4;
                    
                    // Create that child and insert the new particle.
                    G4ThreeVector new_child_min, new_child_max;
                    calculateChildBounds(min, max, center, new_child_idx, new_child_min, new_child_max);
                    if (!node->children[new_child_idx]) node->children[new_child_idx] = std::make_unique<ChargeNode>();
                    insertCharge(node->children[new_child_idx].get(), particle_index, new_child_min, new_child_max);
                }
            }
        }
    } else {
        // This is an internal node. Directly calculate the correct child and recurse.
        G4ThreeVector center = (min + max) * 0.5;
        int child_idx = 0;
        if (fPositions[particle_index].x() >= center.x()) child_idx |= 1;
        if (fPositions[particle_index].y() >= center.y()) child_idx |= 2;
        if (fPositions[particle_index].z() >= center.z()) child_idx |= 4;

        G4ThreeVector child_min, child_max;
        calculateChildBounds(min, max, center, child_idx, child_min, child_max);
        if (!node->children[child_idx]) node->children[child_idx] = std::make_unique<ChargeNode>();
        insertCharge(node->children[child_idx].get(), particle_index, child_min, child_max);
    }

    // After insertion, update the parent node's properties on the way back up the recursion.
    G4double old_total_charge = node->total_charge;
    G4double new_charge = fCharges[particle_index];
    node->total_charge += new_charge;
    
    if (std::abs(node->total_charge) > 1e-25) {
        node->center_of_mass = (node->center_of_mass * old_total_charge + fPositions[particle_index] * new_charge) / node->total_charge;
    } else {
        node->center_of_mass = (min + max) * 0.5;
    }
}

G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldWithApproximation(const G4ThreeVector& point, const ChargeNode* node, const G4ThreeVector& node_min, const G4ThreeVector& node_max) const {
    if (!node || std::abs(node->total_charge) < 1e-25) {
        return G4ThreeVector(0, 0, 0);
    }

    if (node->is_leaf) {
        if (node->particle_index != -1) {
            G4ThreeVector displacement = point - node->center_of_mass;
            G4double d2 = displacement.mag2();
            if (d2 < 1e-16 * mm2) return G4ThreeVector(0,0,0);
            
            G4double inv_d = 1.0 / std::sqrt(d2);
            G4double scale = node->total_charge * k_electric * inv_d * inv_d * inv_d;
            return displacement * scale;
        }
        return G4ThreeVector(0,0,0);
    }

    G4double width = (node_max - node_min).x();
    G4double distance = (point - node->center_of_mass).mag();
    
    if (distance > 0 && (width / distance < barnes_hut_theta_)) {
        G4ThreeVector displacement = point - node->center_of_mass;
        G4double d2 = displacement.mag2();
        if (d2 < 1e-16 * mm2) return G4ThreeVector(0,0,0);
            
        G4double inv_d = 1.0 / std::sqrt(d2);
        G4double scale = node->total_charge * k_electric * inv_d * inv_d * inv_d;
        return displacement * scale;
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

std::unique_ptr<AdaptiveSumRadialFieldMap::Node>
AdaptiveSumRadialFieldMap::createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth) {
    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;

    total_nodes_++;
    max_depth_reached_ = std::max(max_depth_reached_, depth);

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
            
            // The leaf nodes that are refined are removed from the master list.
            // This is complex to do in parallel. A simpler, safe approach is to just
            // leave them in, as they won't be used for final field evaluation.
            // A more robust solution would use a concurrent vector or post-process the list.

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
                
                // Add new leaves to the master list in a thread-safe manner if needed.
                // For now, we are not modifying all_leaves_ after initial creation.
                
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
    
    return (gradient_magnitude / (1e6/m)) > fieldGradThreshold_;
}

void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const {
    const G4ThreeVector r(point[0], point[1], point[2]);
    G4ThreeVector E = evaluateField(r, root_.get());
    field[0] = 0.0; field[1] = 0.0; field[2] = 0.0;
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

// ===================================================================
//  UTILITY AND I/O
// ===================================================================

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

void AdaptiveSumRadialFieldMap::collectStatistics(const Node* node, int depth) {
    // This function is currently just a placeholder.
}

void AdaptiveSumRadialFieldMap::calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                                                     const G4ThreeVector& center, int child_index,
                                                     G4ThreeVector& child_min, G4ThreeVector& child_max) const {
    child_min.set(
        (child_index & 1) ? center.x() : min.x(),
        (child_index & 2) ? center.y() : min.y(),
        (child_index & 4) ? center.z() : min.z()
    );
    child_max.set(
        (child_index & 1) ? max.x() : center.x(),
        (child_index & 2) ? max.y() : center.y(),
        (child_index & 4) ? max.z() : center.z()
    );
}

void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, ("Failed to open file: " + filename).c_str());
        return;
    }
    double world_bounds[6] = { worldMin_.x(), worldMin_.y(), worldMin_.z(), worldMax_.x(), worldMax_.y(), worldMax_.z() };
    int mesh_parameters[5] = { max_depth_, static_cast<int>(minStepSize_ / um), total_nodes_, leaf_nodes_, static_cast<int>(fStorage) };
    outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));
    writeFieldPointsToFile(outfile, root_.get());
    int statistics[3] = { gradient_refinements_, max_depth_reached_, static_cast<int>(fPositions.size()) };
    outfile.write(reinterpret_cast<const char*>(statistics), sizeof(statistics));
    outfile.close();
    G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
}

// void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
//     std::ofstream outfile(filename, std::ios::binary);
//     if (!outfile.is_open()) {
//         G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, ("Failed to open file: " + filename).c_str());
//         return;
//     }

//     // World bounds
//     double world_bounds[6] = {
//         worldMin_.x(), worldMin_.y(), worldMin_.z(),
//         worldMax_.x(), worldMax_.y(), worldMax_.z()
//     };
//     outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));

//     // Mesh parameters
//     int mesh_parameters[5] = {
//         max_depth_,
//         static_cast<int>(minStepSize_ / um),
//         total_nodes_,
//         leaf_nodes_,
//         static_cast<int>(fStorage)
//     };
//     outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));

//     // Write field point data
//     writeFieldPointsToFile(outfile, root_.get());

//     // Statistics metadata
//     int statistics[3] = {
//         gradient_refinements_,
//         max_depth_reached_,
//         static_cast<int>(fPositions.size())
//     };
//     outfile.write(reinterpret_cast<const char*>(statistics), sizeof(statistics));

//     // NEW SECTION: Particle data (positions and charges)
//     size_t num_particles = fPositions.size();
//     outfile.write(reinterpret_cast<const char*>(&num_particles), sizeof(size_t));

//     for (size_t i = 0; i < num_particles; ++i) {
//         double pos[3] = {
//             fPositions[i].x() / meter,
//             fPositions[i].y() / meter,
//             fPositions[i].z() / meter
//         };
//         double charge = fCharges[i] / coulomb;

//         outfile.write(reinterpret_cast<const char*>(pos), sizeof(pos));
//         outfile.write(reinterpret_cast<const char*>(&charge), sizeof(double));
//     }

//     outfile.close();
//     G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
// }


void AdaptiveSumRadialFieldMap::writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const {
    if (!node) return;
    if (node->is_leaf) {
        double position[3] = { node->center.x(), node->center.y(), node->center.z() };
        double field_value[3] = { node->precomputed_field.x(), node->precomputed_field.y(), node->precomputed_field.z() };
        outfile.write(reinterpret_cast<const char*>(position), sizeof(position));
        outfile.write(reinterpret_cast<const char*>(field_value), sizeof(field_value));
    } else {
        for (const auto& child : node->children) {
            writeFieldPointsToFile(outfile, child.get());
        }
    }
}