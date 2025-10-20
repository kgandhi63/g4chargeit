#include "AdaptiveSumRadialFieldMap.hh"
#include "G4Exception.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh" // Includes epsilon0 and eplus
 
#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <stdexcept>
#include <iomanip>
#include <vector>
#include <functional> // Needed if used elsewhere
#include <cstdint>   // Needed if used elsewhere
#include <atomic>    // Needed for atomic counters
 
// Define physical constants locally if needed, but they should come from G4PhysicalConstants
static const double epsilon0_SI  = 8.8541878128e-12 * farad / meter; // F/m
static const G4double k_electric = 1.0 / (4.0 * CLHEP::pi * CLHEP::epsilon0);
 
 
// --- CONSTRUCTOR ---
AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    std::vector<G4ThreeVector>& positions,
    std::vector<G4double>& charges,
    const G4double& gradThreshold,
    const G4double& minStep,
    const G4double& time_step_dt,
    const G4double& material_temp_K,
    const std::string& filename,
    const G4ThreeVector& min_bounds, // Renamed from 'min'
    const G4ThreeVector& max_bounds, // Renamed from 'max'
    int max_depth_param,        // Renamed from 'max_depth'
    StorageType storage)
    : max_depth_(max_depth_param), 
      minStepSize_(minStep),
      worldMin_(min_bounds), 
      worldMax_(max_bounds), 
      fieldGradThreshold_(gradThreshold), 
      fStorage(storage),
      fPositions(positions), 
      fCharges(charges) // Use references directly
{
    // Initialize atomic counters
    total_nodes_.store(0);
    leaf_nodes_.store(0);
    gradient_refinements_.store(0);
    max_depth_reached_.store(0);
    barnes_hut_theta_ = 0.5; // Set default Barnes-Hut theta
 
    // 1. Build initial charge octree
    G4cout << "Building Barnes-Hut charge octree..." << G4endl;
    buildChargeOctree(); 
 
    // 2. Build initial field map octree structure
    G4cout << "Building initial field map octree structure..." << G4endl;
    all_leaves_.clear(); 
    root_ = buildFromScratch(); // Populates all_leaves_ with initial leaves
 
    // 3. Pre-compute field at initial leaf nodes
    G4cout << "Pre-computing field at initial leaf nodes (" << all_leaves_.size() << ")..." << G4endl;
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < all_leaves_.size(); ++i) { 
        Node* leaf = all_leaves_[i];
        if (leaf) {
            // Use initial charges from BH tree
            leaf->precomputed_field = computeFieldFromCharges(leaf->center); 
        }
    }
 
    // 4. Apply charge dissipation (modifies fCharges)
    G4cout << "--> Applying one-time charge dissipation ('tax')..." << G4endl;
    ApplyChargeDissipation(time_step_dt, material_temp_K); 
 
    // 5. Rebuild Barnes-Hut charge octree with dissipated charge
    G4cout << "Rebuild Barnes-Hut charge octree with dissipated charge..." << G4endl;
    buildChargeOctree(); 
 
    // 6. Rebuild initial field map octree structure
    G4cout << "Rebuild initial field map octree structure with dissipated charge..." << G4endl; 
    all_leaves_.clear(); 
    root_ = buildFromScratch(); // Rebuild map structure
 
    // 7. Re-compute field values at initial leaves
    G4cout << "Re-computing field values at initial leaves..." << G4endl; 
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < all_leaves_.size(); ++i) { 
        Node* leaf = all_leaves_[i];
        if(leaf) {
            // Use NEW BH tree
            leaf->precomputed_field = computeFieldFromCharges(leaf->center); 
        }
    }
 
    // 8. Refine field map based on field gradients
    G4cout << "--> Refining field map based on field gradients..." << G4endl; 
#pragma omp parallel
    {
#pragma omp single
        refineMeshByGradient(root_.get(), 0); // Refines based on fields from dissipated charges
    }
 
    // 9. Collect FINAL leaves AFTER refinement (needed for export)
    all_leaves_.clear();
    collectFinalLeaves(root_.get()); 
    leaf_nodes_.store(static_cast<int>(all_leaves_.size())); // Update count based on final leaves
 
    // 10. Export and print stats
    if (!filename.empty()) {
        ExportFieldMapToFile(filename); 
    }
    PrintMeshStatistics(); 
}
 
// --- DESTRUCTOR ---
AdaptiveSumRadialFieldMap::~AdaptiveSumRadialFieldMap() {
    // std::unique_ptr handles memory automatically
}
 
 
// =========================================================================
// === CORE LOGIC FUNCTIONS (Dissipation and Helpers) ===
// =========================================================================
 
// --- ApplyChargeDissipation (Net Charge Based) ---
void AdaptiveSumRadialFieldMap::ApplyChargeDissipation(G4double dt_internal, G4double temp_K) {
    // Calculate conductivity (result should be in SI: S/m)
    double conductivity_SI = calculateConductivity(temp_K);
 
    // Explicit SI value for epsilon0 (F/m)
    const double epsilon0_SI_Value = 8.8541878128e-12; // Numerical value in F/m
 
    // Calculate the rate constant (ensure it's a dimensionless number representing 1/s)
    // (S/m) / (F/m) = (1/s)
    double rate_constant_Value = conductivity_SI / epsilon0_SI_Value; 
 
    G4double total_dissipated_charge = 0.0;
 
#pragma omp parallel for schedule(dynamic) reduction(+:total_dissipated_charge)
    for (const auto& leaf : all_leaves_) {
        if (!leaf) continue;
 
        double Q_node_net_Internal = 0.0; // Units: e+ (internal charge unit)
        std::vector<int> positive_particle_indices;
        std::vector<int> negative_particle_indices;
 
        // 1. Find net charge and particles in this leaf node
        for (size_t i = 0; i < fPositions.size(); ++i) {
            if (pointInside(leaf->min, leaf->max, fPositions[i])) {
                Q_node_net_Internal += fCharges[i]; // Accumulate charge in internal units (e+)
                
                // Identify positive/negative particles for dissipation
                if (fCharges[i] > 1e-21 * CLHEP::eplus) { 
                    positive_particle_indices.push_back(static_cast<int>(i)); 
                }
                else if (fCharges[i] < -1e-21 * CLHEP::eplus) { 
                    negative_particle_indices.push_back(static_cast<int>(i)); 
                }
            }
        }
 
        // If net charge is negligible, continue
        if (std::abs(Q_node_net_Internal) < 1e-21 * CLHEP::eplus) continue;
 
        // 2. Calculate change (tau relaxation formula: dQ = - (sigma/epsilon0) * Q * dt)
        double Q_node_net_Value = Q_node_net_Internal / CLHEP::eplus; // Dimensionless number of elementary charges
        double dt_Value = dt_internal; // Dimensionless number of seconds
 
        double delta_Q_node_Value = -rate_constant_Value * Q_node_net_Value * dt_Value;
 
        double delta_Q_node = delta_Q_node_Value * CLHEP::eplus; // Result now has units of e+
 
        // 3. Distribute the change
        double charge_change_magnitude = std::abs(delta_Q_node); // Still in e+ units
 
        if (charge_change_magnitude > 0) {
            total_dissipated_charge += charge_change_magnitude; // Accumulate magnitude in e+
 
            if (delta_Q_node < 0 && !positive_particle_indices.empty()) {
                // Net charge is positive -> needs to decrease -> remove from positive particles
                distributeChargeChange(positive_particle_indices, -charge_change_magnitude);
            } 
            else if (delta_Q_node > 0 && !negative_particle_indices.empty()) {
                // Net charge is negative -> needs to increase -> remove from negative particles
                distributeChargeChange(negative_particle_indices, charge_change_magnitude);
            }
        }
    } // End of parallel loop
 
    G4cout << "   Charge dissipation applied (neutralizing)." << G4endl;
    G4cout << "   >>> Total charge dissipated/neutralized this step: "
           << total_dissipated_charge / CLHEP::eplus << " e <<<" << G4endl;
}
 
// --- calculateConductivity ---
double AdaptiveSumRadialFieldMap::calculateConductivity(double temp_K) const {
    if (temp_K <= 0) return 0.0;
    // Formula from user's source image (empirical fit for material)
    return 6.0e-18 * std::exp(0.0230 * temp_K);
}
 
// --- distributeChargeChange ---
void AdaptiveSumRadialFieldMap::distributeChargeChange(const std::vector<int>& particle_indices, G4double total_charge_change) {
    // Check for invalid inputs
    if (particle_indices.empty() || std::abs(total_charge_change) < 1e-25 * CLHEP::coulomb) {
        return; 
    }
 
    // Calculate change per particle for this specific group
    double charge_change_per_particle = total_charge_change / particle_indices.size();
 
    for (int idx : particle_indices) {
        // Check index bounds for safety
        if (idx >= 0 && static_cast<size_t>(idx) < fCharges.size()) {
            
            // 1. Modify the master charge list atomically 
            #pragma omp atomic update
            fCharges[idx] += charge_change_per_particle;
 
            // 2. Clamp to zero if sign flips due to overshoot/error
            G4double current_charge;
            #pragma omp atomic read
            current_charge = fCharges[idx]; 
 
            bool sign_flipped = 
                (charge_change_per_particle < 0 && current_charge < 1e-21 * CLHEP::eplus) || // Was positive, went negative
                (charge_change_per_particle > 0 && current_charge > -1e-21 * CLHEP::eplus);  // Was negative, went positive
            
            if (sign_flipped) { 
                #pragma omp atomic write
                fCharges[idx] = 0.0;
            }
 
        } else {
            // Use critical section for thread-safe error output
            #pragma omp critical (error_log) 
            G4cerr << "Warning: Invalid particle index (" << idx << ") in distributeChargeChange." << G4endl;
        }
    }
}
 
 
// =========================================================================
// === BARNES-HUT IMPLEMENTATION (Readability Refactor) ===
// =========================================================================
 
// --- buildChargeOctree ---
void AdaptiveSumRadialFieldMap::buildChargeOctree() {
    charge_root_.reset(); // Important when rebuilding
    if (fPositions.empty()) return;
    
    G4ThreeVector min_box, max_box; 
    calculateBoundingBox(min_box, max_box);
    
    charge_root_ = std::make_unique<ChargeNode>();
    charge_root_->is_leaf = true; 
    charge_root_->particle_index = -1; 
    charge_root_->total_charge = 0.0; 
    charge_root_->center_of_mass = (min_box + max_box) * 0.5;
    
    for (size_t i = 0; i < fPositions.size(); ++i) {
        if (std::abs(fCharges[i]) > 1e-25 * CLHEP::coulomb && 
            pointInside(min_box, max_box, fPositions[i])) {
            insertCharge(charge_root_.get(), static_cast<int>(i), min_box, max_box);
        }
    }
}
 
// --- insertCharge ---
void AdaptiveSumRadialFieldMap::insertCharge(
    ChargeNode* node, 
    int particle_index, 
    const G4ThreeVector& min_bounds, 
    const G4ThreeVector& max_bounds) 
{
    if (!node) return; 
    
    if (particle_index < 0 || static_cast<size_t>(particle_index) >= fPositions.size()) {
        G4Exception("AdaptiveSumRadialFieldMap::insertCharge", "IndexOutOfBounds", FatalException, "Invalid particle index provided."); 
        return;
    }
    
    // Update center of mass (CoM) and total charge
    G4double old_total_charge = node->total_charge; 
    G4double charge_to_add = fCharges[particle_index]; 
    node->total_charge += charge_to_add;
    
    if (std::abs(node->total_charge) > 1e-25 * CLHEP::coulomb) { 
        if (std::abs(old_total_charge) > 1e-25 * CLHEP::coulomb) { 
            // Calculate new CoM using weighted average
            node->center_of_mass = (node->center_of_mass * old_total_charge + 
                                    fPositions[particle_index] * charge_to_add) / node->total_charge; 
        } else { 
            node->center_of_mass = fPositions[particle_index]; 
        } 
    } else { 
        // Net charge is zero, center CoM in the middle of the box
        node->center_of_mass = (min_bounds + max_bounds) * 0.5; 
    }
    
    // Handle leaf node insertion
    if (node->is_leaf) {
        if (node->particle_index == -1) { 
            // Empty leaf: place particle here
            node->particle_index = particle_index; 
        }
        else {
            // Occupied leaf: need to split
            int old_particle_index = node->particle_index; 
            bool aggregate = false;
            
            if (old_particle_index == -2) { 
                aggregate = true; 
            } 
            else if (old_particle_index >= 0) { 
                // Check if particles are at the same location (prevent infinite split)
                if (fPositions[particle_index] == fPositions[old_particle_index]) { 
                    aggregate = true; 
                } else { 
                    // Check if box is too small (prevent infinite split)
                    const G4double minBoxSizeSq = (1.0 * nm) * (1.0 * nm); 
                    if ((max_bounds - min_bounds).mag2() < minBoxSizeSq) { 
                        aggregate = true; 
                    } 
                } 
            }
            
            if (aggregate && old_particle_index != -2) { 
                node->particle_index = -2; // Mark as aggregated (multiple particles)
                return; 
            }
            
            // Convert to internal node
            node->is_leaf = false; 
            node->particle_index = -1; // No longer holds a single particle index
            G4ThreeVector center = (min_bounds + max_bounds) * 0.5;
            
            // Insert the old particle into a child node
            if (old_particle_index >= 0) {
                 int old_child_idx = 0; 
                 if (fPositions[old_particle_index].x() >= center.x()) old_child_idx |= 1; 
                 if (fPositions[old_particle_index].y() >= center.y()) old_child_idx |= 2; 
                 if (fPositions[old_particle_index].z() >= center.z()) old_child_idx |= 4;
                 
                 G4ThreeVector old_min, old_max; 
                 calculateChildBounds(min_bounds, max_bounds, center, old_child_idx, old_min, old_max); 
                 
                 if (!node->children[old_child_idx]) {
                     node->children[old_child_idx] = std::make_unique<ChargeNode>(); 
                 }
                 insertCharge(node->children[old_child_idx].get(), old_particle_index, old_min, old_max);
            }
            
            // Insert the new particle into a child node
            int new_child_idx = 0; 
            if (fPositions[particle_index].x() >= center.x()) new_child_idx |= 1; 
            if (fPositions[particle_index].y() >= center.y()) new_child_idx |= 2; 
            if (fPositions[particle_index].z() >= center.z()) new_child_idx |= 4;
            
            G4ThreeVector new_min, new_max; 
            calculateChildBounds(min_bounds, max_bounds, center, new_child_idx, new_min, new_max); 
            
            if (!node->children[new_child_idx]) {
                node->children[new_child_idx] = std::make_unique<ChargeNode>(); 
            }
            insertCharge(node->children[new_child_idx].get(), particle_index, new_min, new_max);
        }
    } 
    else { 
        // Internal node: find correct child and recurse
        G4ThreeVector center = (min_bounds + max_bounds) * 0.5; 
        int child_idx = 0; 
        if (fPositions[particle_index].x() >= center.x()) child_idx |= 1; 
        if (fPositions[particle_index].y() >= center.y()) child_idx |= 2; 
        if (fPositions[particle_index].z() >= center.z()) child_idx |= 4; 
        
        G4ThreeVector c_min, c_max; 
        calculateChildBounds(min_bounds, max_bounds, center, child_idx, c_min, c_max); 
        
        if (!node->children[child_idx]) {
            node->children[child_idx] = std::make_unique<ChargeNode>(); 
        }
        insertCharge(node->children[child_idx].get(), particle_index, c_min, c_max); 
    }
}
 
// --- computeFieldWithApproximation ---
G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldWithApproximation(
    const G4ThreeVector& point, 
    const ChargeNode* node, 
    const G4ThreeVector& node_min, 
    const G4ThreeVector& node_max) const 
{
    if (!node || std::abs(node->total_charge) < 1e-25 * CLHEP::coulomb) {
        return G4ThreeVector(0,0,0);
    }
    
    G4ThreeVector displacement = point - node->center_of_mass; 
    G4double d2 = displacement.mag2(); 
    const G4double softening_factor_sq = (1.0*nm)*(1.0*nm); 
    
    if (d2 < softening_factor_sq) {
        d2 = softening_factor_sq; // Apply softening
    }
    
    if (node->is_leaf) { 
        if (node->particle_index != -1) { 
            // Treat as a single point charge at CoM
            G4double inv_d3 = 1.0 / (std::sqrt(d2) * d2); 
            G4double scale = node->total_charge * k_electric * inv_d3; 
            return displacement * scale; 
        } else {
            return G4ThreeVector(0,0,0); 
        }
    }
    
    // Check Barnes-Hut criterion (width / distance < theta)
    double width = (node_max.x() - node_min.x()); 
    double distance = std::sqrt(d2); 
    
    if (width / distance < barnes_hut_theta_) { 
        // Approximation is acceptable: treat as a single point charge
        G4double inv_d3 = 1.0 / (distance * d2); 
        G4double scale = node->total_charge * k_electric * inv_d3; 
        return displacement * scale; 
    }
    else { 
        // Recursively compute contributions from children
        G4ThreeVector E_total(0,0,0); 
        G4ThreeVector center = (node_min + node_max) * 0.5; 
        
        for(int i = 0; i < 8; ++i) { 
            if(node->children[i]) { 
                G4ThreeVector c_min, c_max; 
                calculateChildBounds(node_min, node_max, center, i, c_min, c_max); 
                E_total += computeFieldWithApproximation(point, node->children[i].get(), c_min, c_max); 
            } 
        } 
        return E_total; 
    }
}
 
// --- computeFieldFromCharges ---
G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const { 
    if (!charge_root_) return G4ThreeVector(0,0,0); 
    G4ThreeVector min_box, max_box; 
    calculateBoundingBox(min_box, max_box); 
    
    return computeFieldWithApproximation(point, charge_root_.get(), min_box, max_box); 
}
 
 
// =========================================================================
// === FIELD MAP OCTREE LOGIC (Readability Refactor) ===
// =========================================================================
 
// --- buildFromScratch ---
std::unique_ptr<AdaptiveSumRadialFieldMap::Node> AdaptiveSumRadialFieldMap::buildFromScratch() {
    G4ThreeVector min_box, max_box; 
    calculateBoundingBox(min_box, max_box); 
    
    auto root_node = std::make_unique<Node>(); 
    root_node->min = min_box; 
    root_node->max = max_box; 
    root_node->center = (min_box + max_box) * 0.5; 
    root_node->is_leaf = true; 
    
    total_nodes_.store(1); 
    leaf_nodes_.store(1); 
    max_depth_reached_.store(0); 
    all_leaves_.push_back(root_node.get()); 
    
    return root_node;
}
 
// --- createOctreeFromScratch (unused helper, but formatted) ---
std::unique_ptr<AdaptiveSumRadialFieldMap::Node> AdaptiveSumRadialFieldMap::createOctreeFromScratch(
    const G4ThreeVector& min_bounds, 
    const G4ThreeVector& max_bounds, 
    int depth) 
{
    auto node = std::make_unique<Node>(); 
    node->min = min_bounds; 
    node->max = max_bounds; 
    node->center = (min_bounds + max_bounds) * 0.5; 
    node->is_leaf = true; 
    
    total_nodes_++;
    
    // Proper atomic update for max_depth_reached_
    int current_max = max_depth_reached_.load(std::memory_order_relaxed);
    while (depth > current_max) {
        if (max_depth_reached_.compare_exchange_weak(current_max, depth, std::memory_order_relaxed)) {
            break;
        }
        // If exchange failed, current_max was updated, loop again
    }
    
    leaf_nodes_++; 
    return node;
}
 
// --- refineMeshByGradient ---
void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) {
    if (!node) return;
    
    // Update max_depth_reached_ atomically
    int current_max = max_depth_reached_.load(std::memory_order_relaxed);
    while (depth > current_max) {
        if (max_depth_reached_.compare_exchange_weak(current_max, depth, std::memory_order_relaxed)) {
            break;
        }
    }
    
    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x()); 
        
        // Check refinement criteria
        if (depth < max_depth_ && 
            size > minStepSize_ && 
            hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2)) 
        {

            gradient_refinements_++; 
            node->is_leaf = false;
            leaf_nodes_--;
            
            // Create 8 child nodes
            for (int i = 0; i < 8; ++i) { 
                G4ThreeVector c_min, c_max; 
                calculateChildBounds(node->min, node->max, node->center, i, c_min, c_max); 
                
                auto child = std::make_unique<Node>(); 
                child->min = c_min; 
                child->max = c_max; 
                child->center = (c_min + c_max) * 0.5; 
                child->is_leaf = true; 
                child->precomputed_field = computeFieldFromCharges(child->center); 
                
                total_nodes_++;
                leaf_nodes_++;

                node->children[i] = std::move(child); 
            }
            
            // Recursively refine children (using OpenMP tasks/taskwait as originally structured)
            for (int i = 0; i < 8; ++i) { 
                if(node->children[i]) {
                    #pragma omp task
                    refineMeshByGradient(node->children[i].get(), depth + 1); 
                } 
            }
            #pragma omp taskwait
        }
    } 
    else { 
        // Already internal node, keep recursing down
        for (int i = 0; i < 8; ++i) { 
            if(node->children[i]) {
                #pragma omp task
                refineMeshByGradient(node->children[i].get(), depth + 1); 
            } 
        }
        #pragma omp taskwait
   }
}
 
// --- hasHighFieldGradient ---
bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(
    const G4ThreeVector& center, 
    const G4ThreeVector& center_field [[maybe_unused]], 
    double sample_distance) const 
{
    if (fPositions.empty()) return false; 
    
    // Clamp the sampling distance to a reasonable range
    double actual_distance = std::min(std::max(sample_distance, 0.1 * nm), 1.0 * um); 
    
    // Define 6 sampling points around the center
    const std::vector<G4ThreeVector> sample_points = { 
        center + G4ThreeVector(actual_distance, 0, 0), 
        center - G4ThreeVector(actual_distance, 0, 0), 
        center + G4ThreeVector(0, actual_distance, 0), 
        center - G4ThreeVector(0, actual_distance, 0), 
        center + G4ThreeVector(0, 0, actual_distance), 
        center - G4ThreeVector(0, 0, actual_distance) 
    }; 
    std::vector<double> field_magnitudes(6);

    // double actual_distance = std::min(sample_distance, 2.0 * um);
    
    // const std::vector<G4ThreeVector> sample_points = {
    //     center + G4ThreeVector(actual_distance, 0, 0), center - G4ThreeVector(actual_distance, 0, 0),
    //     center + G4ThreeVector(0, actual_distance, 0), center - G4ThreeVector(0, actual_distance, 0),
    //     center + G4ThreeVector(0, 0, actual_distance), center - G4ThreeVector(0, 0, actual_distance)
    // };
    
    // std::vector<double> field_magnitudes(6);
    
    // Compute field magnitude at all 6 points in parallel
    #pragma omp parallel for 
    for (int i = 0; i < 6; ++i) { 
        field_magnitudes[i] = computeFieldFromCharges(sample_points[i]).mag(); 
    }
    
    if (actual_distance == 0) return false; 
    
    // Use central difference method to estimate gradient components
    double inv_2d = 1.0/(2.0*actual_distance); 
    double gx = (field_magnitudes[0] - field_magnitudes[1]) * inv_2d; 
    double gy = (field_magnitudes[2] - field_magnitudes[3]) * inv_2d; 
    double gz = (field_magnitudes[4] - field_magnitudes[5]) * inv_2d; 
    double grad_sq = gx*gx + gy*gy + gz*gz;
    
    // Check if squared gradient is above the squared threshold (to avoid sqrt)
    return grad_sq > (fieldGradThreshold_ * fieldGradThreshold_);
}

 // --- collectFinalLeaves ---
void AdaptiveSumRadialFieldMap::collectFinalLeaves(Node* node) { 
    if (!node) return; 
    
    if (node->is_leaf) { 
        all_leaves_.push_back(node); 
    } 
    else { 
        for (int i = 0; i < 8; ++i) { 
            if(node->children[i]) {
                collectFinalLeaves(node->children[i].get()); 
            }
        } 
    } 
}
 
 
// =========================================================================
// === FIELD EVALUATION (Readability Refactor) ===
// =========================================================================
 
// --- GetFieldValue (GEANT4 interface) ---
void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const { 
    const G4ThreeVector r(point[0], point[1], point[2]); 
    G4ThreeVector E = evaluateField(r); 
    
    // Magnetic components
    field[0] = 0; 
    field[1] = 0; 
    field[2] = 0; 
    // Electric components
    field[3] = E.x(); 
    field[4] = E.y(); 
    field[5] = E.z(); 
}
 
// --- evaluateField (Entry point) ---
G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point) const { 
    if (!pointInside(worldMin_, worldMax_, point)) return G4ThreeVector(0,0,0); 
    if (!root_) return G4ThreeVector(0,0,0); 
    
    return evaluateFieldRecursive(point, root_.get()); 
}
 
// --- evaluateFieldRecursive ---
G4ThreeVector AdaptiveSumRadialFieldMap::evaluateFieldRecursive(
    const G4ThreeVector& point, 
    const Node* node) const 
{
    if (!node) return G4ThreeVector(0,0,0);
    
    // If leaf, return pre-computed value
    if (node->is_leaf) return node->precomputed_field;
    
    // If internal node, determine which child the point falls into
    G4ThreeVector center = node->center;
    int child_idx = 0;
    if (point.x() >= center.x()) child_idx |= 1;
    if (point.y() >= center.y()) child_idx |= 2;
    if (point.z() >= center.z()) child_idx |= 4;
    
    if (node->children[child_idx]) {
        return evaluateFieldRecursive(point, node->children[child_idx].get());
    } 
    else {
        // Warning if a point falls into a region that hasn't been refined/created
        #pragma omp critical (error_log)
        G4cerr << "Warning: Point in internal node missing child during eval." << G4endl;
        return G4ThreeVector(0,0,0);
    }
}
 
 
// =========================================================================
// === UTILITY AND I/O (Readability Refactor) ===
// =========================================================================
 
// --- pointInside ---
bool AdaptiveSumRadialFieldMap::pointInside(
    const G4ThreeVector& min_bounds, 
    const G4ThreeVector& max_bounds, 
    const G4ThreeVector& point) const 
{ 
    return (point.x() >= min_bounds.x() && point.x() <= max_bounds.x() && 
            point.y() >= min_bounds.y() && point.y() <= max_bounds.y() && 
            point.z() >= min_bounds.z() && point.z() <= max_bounds.z()); 
}
 
// --- calculateBoundingBox ---
void AdaptiveSumRadialFieldMap::calculateBoundingBox(G4ThreeVector& min_box, G4ThreeVector& max_box) const { 
    min_box = worldMin_; 
    max_box = worldMax_; 
}
 
// --- PrintMeshStatistics ---
void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const { 
    G4cout << "\n=== Adaptive Mesh Statistics ===" << G4endl; 
    G4cout << "Total nodes created:      " << total_nodes_.load() << G4endl; 
    G4cout << "Final leaf nodes:         " << leaf_nodes_.load() << G4endl; 
    G4cout << "Max octree depth reached: " << max_depth_reached_.load() << G4endl; 
    G4cout << "Gradient refinements:     " << gradient_refinements_.load() << G4endl; 
    G4cout << "=================================\n" << G4endl; 
}
 
// --- calculateChildBounds ---
void AdaptiveSumRadialFieldMap::calculateChildBounds(
    const G4ThreeVector& p_min, 
    const G4ThreeVector& p_max, 
    const G4ThreeVector& p_cen, 
    int c_idx, 
    G4ThreeVector& c_min, 
    G4ThreeVector& c_max) const 
{ 
    // Determine child min coordinates
    c_min.setX((c_idx & 1) ? p_cen.x() : p_min.x()); 
    c_min.setY((c_idx & 2) ? p_cen.y() : p_min.y()); 
    c_min.setZ((c_idx & 4) ? p_cen.z() : p_min.z()); 
    
    // Determine child max coordinates
    c_max.setX((c_idx & 1) ? p_max.x() : p_cen.x()); 
    c_max.setY((c_idx & 2) ? p_max.y() : p_cen.y()); 
    c_max.setZ((c_idx & 4) ? p_max.z() : p_cen.z()); 
}
 
// // --- ExportFieldMapToFile ---
// void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
//     std::ofstream outfile(filename, std::ios::binary | std::ios::trunc);
    
//     if (!outfile.is_open()) {
//         G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, 
//                     ("Failed to open file for writing: " + filename).c_str());
//         return;
//     }
    
//     // 1. Write metadata (file version and world bounds)
//     uint32_t file_version = 2; 
//     outfile.write(reinterpret_cast<const char*>(&file_version), sizeof(file_version));
    
//     double world_bounds[6] = { worldMin_.x(), worldMin_.y(), worldMin_.z(), 
//                                worldMax_.x(), worldMax_.y(), worldMax_.z() }; 
//     outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    
//     // 2. Write Octree parameters
//     uint32_t max_d = static_cast<uint32_t>(max_depth_); 
//     double min_s = minStepSize_; 
//     uint32_t storage_type = static_cast<uint32_t>(fStorage);
//     uint64_t final_leaf_count = static_cast<uint64_t>(all_leaves_.size());
    
//     outfile.write(reinterpret_cast<const char*>(&max_d), sizeof(max_d)); 
//     outfile.write(reinterpret_cast<const char*>(&min_s), sizeof(min_s)); 
//     outfile.write(reinterpret_cast<const char*>(&storage_type), sizeof(storage_type)); 
//     outfile.write(reinterpret_cast<const char*>(&final_leaf_count), sizeof(final_leaf_count));
    
//     // 3. Write leaf node data (center, field, size)
//     for (const Node* leaf : all_leaves_) { 
//         if (!leaf) continue; 
        
//         float pos[3] = { static_cast<float>(leaf->center.x()), 
//                          static_cast<float>(leaf->center.y()), 
//                          static_cast<float>(leaf->center.z()) }; 
//         outfile.write(reinterpret_cast<const char*>(pos), sizeof(pos)); 
        
//         float field[3] = { static_cast<float>(leaf->precomputed_field.x()), 
//                            static_cast<float>(leaf->precomputed_field.y()), 
//                            static_cast<float>(leaf->precomputed_field.z()) }; 
//         outfile.write(reinterpret_cast<const char*>(field), sizeof(field)); 
        
//         float size = static_cast<float>(leaf->max.x() - leaf->min.x()); 
//         outfile.write(reinterpret_cast<const char*>(&size), sizeof(size)); 
//     }
    
//     // 4. Write particle data
//     uint64_t num_particles = static_cast<uint64_t>(fPositions.size()); 
//     outfile.write(reinterpret_cast<const char*>(&num_particles), sizeof(num_particles));
    
//     for (size_t i = 0; i < num_particles; ++i) { 
//         double pos[3] = { fPositions[i].x(), fPositions[i].y(), fPositions[i].z() }; 
//         outfile.write(reinterpret_cast<const char*>(pos), sizeof(pos)); 
        
//         double charge = fCharges[i]; 
//         outfile.write(reinterpret_cast<const char*>(&charge), sizeof(charge)); 
//     }
    
//     // 5. Write final statistics
//     uint32_t grad_ref = static_cast<uint32_t>(gradient_refinements_.load()); 
//     uint32_t max_depth_r = static_cast<uint32_t>(max_depth_reached_.load());
    
//     outfile.write(reinterpret_cast<const char*>(&grad_ref), sizeof(grad_ref)); 
//     outfile.write(reinterpret_cast<const char*>(&max_depth_r), sizeof(max_depth_r));
    
//     outfile.close(); 
//     G4cout << "Adaptive binary field map exported..." << G4endl;
// }

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