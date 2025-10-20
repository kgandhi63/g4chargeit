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
#include <cstdint>    // Needed if used elsewhere
#include <atomic>     // Needed for atomic counters

// =========================================================================
// PHYSICAL CONSTANTS & DEFINITIONS
// =========================================================================

// Define physical constants locally if needed, but they should come from G4PhysicalConstants
static const double epsilon0_SI = 8.8541878128e-12 * CLHEP::farad / CLHEP::meter; // F/m
static const G4double k_electric = 1.0 / (4.0 * CLHEP::pi * CLHEP::epsilon0);

// =========================================================================
// CONSTRUCTOR / DESTRUCTOR
// =========================================================================

// --- CONSTRUCTOR (Unaltered Sequence from your input) ---
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
    int max_depth_param,            // Renamed from 'max_depth'
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
    // Use store for atomic initialization
    total_nodes_.store(0);
    leaf_nodes_.store(0);
    gradient_refinements_.store(0);
    max_depth_reached_.store(0); // Use atomic for max_depth_reached_
    barnes_hut_theta_ = 0.5;    // Set default Barnes-Hut theta

    // Assuming setWorldDimensions exists and is needed
    // setWorldDimensions(worldMax_.x() - worldMin_.x(), worldMax_.y() - worldMin_.y(), worldMax_.z() - worldMin_.z());

    G4cout << "Building Barnes-Hut charge octree..." << G4endl;
    buildChargeOctree(); // Based on original charges

    G4cout << "Building initial field map octree structure..." << G4endl;
    all_leaves_.clear();              // Start with empty list
    root_ = buildFromScratch();       // Populates all_leaves_ with initial leaves

    G4cout << "Pre-computing field at initial leaf nodes (" << all_leaves_.size() << ")..." << G4endl;
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < all_leaves_.size(); ++i) { // Use index loop
        Node* leaf = all_leaves_[i];
        if (leaf) {
            leaf->precomputed_field = computeFieldFromCharges(leaf->center); // Use initial charges
        }
    }

    // --- Apply charge dissipation based on INITIAL mesh structure ---
    // --- THIS IS WHERE DISSIPATION HAPPENS IN YOUR ORIGINAL SEQUENCE ---
    G4cout << "--> Applying one-time charge dissipation ('tax')..." << G4endl;
    ApplyChargeDissipation(time_step_dt, material_temp_K); // CALLS THE *NEW* DISSIPATION CODE
    // The master charge list (fCharges) has now been MODIFIED.

    // --- Continue with the rest of your original constructor sequence ---
    G4cout << "Rebuild Barnes-Hut charge octree with dissipated charge..." << G4endl;
    buildChargeOctree(); // Rebuild BH tree with NEW fCharges

    G4cout << "rebuild initial field map octree structure with dissipated charge..." << G4endl; // Log clarification
    all_leaves_.clear();              // Clear again before rebuilding map structure
    root_ = buildFromScratch();       // Rebuild map structure

    G4cout << "Re-computing field values at initial leaves..." << G4endl; // Log clarification
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < all_leaves_.size(); ++i) { // Use index loop
        Node* leaf = all_leaves_[i];
        if (leaf) {
            leaf->precomputed_field = computeFieldFromCharges(leaf->center); // Use NEW BH tree
        }
    }

    G4cout << "--> Refining field map based on field gradients..." << G4endl; // Log clarification
#pragma omp parallel
    {
#pragma omp single
        refineMeshByGradient(root_.get(), 0); // Refines based on fields from dissipated charges
    }

    // --- Collect FINAL leaves AFTER refinement (needed for export) ---
    all_leaves_.clear();
    collectFinalLeaves(root_.get()); // Collect final leaves
    leaf_nodes_.store(static_cast<int>(all_leaves_.size())); // Update count based on final leaves

    // collectStatistics might be redundant
    // collectStatistics(root_.get(), 0);

    if (!filename.empty()) {
        ExportFieldMapToFile(filename); // Export the final refined map
    }
    PrintMeshStatistics(); // Print final stats
}

// --- Destructor (If needed, likely empty with unique_ptr) ---
AdaptiveSumRadialFieldMap::~AdaptiveSumRadialFieldMap() {
    // std::unique_ptr handles memory automatically
}

// =========================================================================
// CHARGE DISSIPATION LOGIC
// =========================================================================

// --- REVISED ApplyChargeDissipation (Net Charge Based) ---
// --- REVISED ApplyChargeDissipation with Explicit Unit Stripping ---
void AdaptiveSumRadialFieldMap::ApplyChargeDissipation(G4double dt_internal, G4double temp_K) 
{ // Rename dt for clarity

    // Calculate conductivity (result should be in SI: S/m)
    double conductivity_SI = calculateConductivity(temp_K);

    // Explicit SI value for epsilon0 (F/m)
    const double epsilon0_SI_Value = 8.8541878128e-12; // Numerical value in F/m

    // Calculate the rate constant (ensure it's a dimensionless number representing 1/s)
    // (S/m) / (F/m) = 1/s
    double rate_constant_Value = conductivity_SI / epsilon0_SI_Value; 

    G4double total_dissipated_charge = 0.0;

#pragma omp parallel for schedule(dynamic) reduction(+:total_dissipated_charge)
    for (const auto& leaf : all_leaves_) {
        if (!leaf) continue;

        double Q_node_net_Internal = 0.0; // Units: e+ (internal charge unit)
        std::vector<int> positive_particle_indices;
        std::vector<int> negative_particle_indices;

        for (size_t i = 0; i < fPositions.size(); ++i) {
            if (pointInside(leaf->min, leaf->max, fPositions[i])) {
                Q_node_net_Internal += fCharges[i]; // Accumulate charge in internal units (e+)
                
                if (fCharges[i] > 1e-21 * CLHEP::eplus) { 
                    positive_particle_indices.push_back(static_cast<int>(i)); 
                } else if (fCharges[i] < -1e-21 * CLHEP::eplus) { 
                    negative_particle_indices.push_back(static_cast<int>(i)); 
                }
            }
        }

        if (std::abs(Q_node_net_Internal) < 1e-21 * CLHEP::eplus) continue;

        // --- Strip units explicitly before calculation ---
        double Q_node_net_Value = Q_node_net_Internal / CLHEP::eplus; // Dimensionless number of elementary charges
        double dt_Value = dt_internal; // Dimensionless number of seconds

        double delta_Q_node_Value = -rate_constant_Value * Q_node_net_Value * dt_Value;

        // --- Re-apply the unit at the end ---
        double delta_Q_node = delta_Q_node_Value * CLHEP::eplus; // Result now has units of e+
        //---------------------------------------------------------

        double charge_change_magnitude = std::abs(delta_Q_node); // Still in e+ units

        if (charge_change_magnitude > 0) {
            total_dissipated_charge += charge_change_magnitude; // Accumulate magnitude in e+

            if (delta_Q_node < 0 && !positive_particle_indices.empty()) {
                distributeChargeChange(positive_particle_indices, -charge_change_magnitude);
            } else if (delta_Q_node > 0 && !negative_particle_indices.empty()) {
                distributeChargeChange(negative_particle_indices, charge_change_magnitude);
            }
        }
    } // End of parallel loop

    G4cout << "   Charge dissipation applied (neutralizing)." << G4endl;
    G4cout << "   >>> Total charge dissipated/neutralized this step: "
           << total_dissipated_charge / CLHEP::eplus << " e <<<" << G4endl;
}

// --- calculateConductivity remains the same as your input ---
double AdaptiveSumRadialFieldMap::calculateConductivity(double temp_K) const 
{
    if (temp_K <= 0) return 0.0;
    // Formula from user's source image
    return 6.0e-18 * std::exp(0.0230 * temp_K);
}

// --- NEW UNIFIED HELPER FUNCTION: distributeChargeChange ---
// Replaces the old removeChargeFromRegion function
void AdaptiveSumRadialFieldMap::distributeChargeChange(
    const std::vector<int>& particle_indices, 
    G4double total_charge_change) 
{
    // Check for invalid inputs
    if (particle_indices.empty() || std::abs(total_charge_change) < 1e-25 * CLHEP::coulomb) return; // Use smaller threshold

    // Calculate change per particle for this specific group
    double charge_change_per_particle = total_charge_change / particle_indices.size();

    // Optional Debug Output (remove in production)
    // G4cout << "    Distributing " << G4BestUnit(charge_change_per_particle,"Electric charge") << " change to each of " << particle_indices.size() << " particles." << G4endl;

    for (int idx : particle_indices) {
        // Check index bounds for safety
        if (idx >= 0 && static_cast<size_t>(idx) < fCharges.size()) {
            // Modify the master charge list atomically (+= handles both addition and subtraction)
#pragma omp atomic update
            fCharges[idx] += charge_change_per_particle;

            // Ensure charge doesn't cross zero due to overshoot or numerical error
            G4double current_charge;
#pragma omp atomic read
            current_charge = fCharges[idx]; // Read potentially updated value

            // Check if the sign effectively flipped past zero
            // Clamp to zero using atomic write for safety
            bool should_clamp_to_zero = 
                (charge_change_per_particle < 0 && current_charge < 1e-21 * CLHEP::eplus) || // Went negative
                (charge_change_per_particle > 0 && current_charge > -1e-21 * CLHEP::eplus);  // Went positive

            if (should_clamp_to_zero) {
#pragma omp atomic write
                fCharges[idx] = 0.0;
            }
        } else {
#pragma omp critical (error_log) // Use critical section for thread-safe error output
            G4cerr << "Warning: Invalid particle index (" << idx << ") in distributeChargeChange." << G4endl;
        }
    }
}

// --- OLD removeChargeFromRegion function IS NO LONGER DEFINED ---
// void AdaptiveSumRadialFieldMap::removeChargeFromRegion(...) { /* DELETED */ }

// =========================================================================
// BARNES-HUT IMPLEMENTATION (Unaltered from your input)
// =========================================================================

void AdaptiveSumRadialFieldMap::buildChargeOctree() 
{
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
            pointInside(min_box, max_box, fPositions[i])) 
        {
            insertCharge(charge_root_.get(), static_cast<int>(i), min_box, max_box);
        }
    }
}

void AdaptiveSumRadialFieldMap::insertCharge(
    ChargeNode* node, 
    int particle_index, 
    const G4ThreeVector& min_bounds, 
    const G4ThreeVector& max_bounds) 
{
    // *** IMPORTANT: Fill in the G4Exception call properly ***
    if (!node) return; 
    
    if (particle_index < 0 || static_cast<size_t>(particle_index) >= fPositions.size()) {
        G4Exception("AdaptiveSumRadialFieldMap::insertCharge", "IndexOutOfBounds", FatalException, "Invalid particle index provided."); 
        return;
    }
    
    // --- Update total charge and Center of Mass (CoM) ---
    G4double old_total_charge = node->total_charge; 
    G4double charge_to_add = fCharges[particle_index]; 
    node->total_charge += charge_to_add;
    
    if (std::abs(node->total_charge) > 1e-25 * CLHEP::coulomb) { 
        if (std::abs(old_total_charge) > 1e-25 * CLHEP::coulomb) { 
            node->center_of_mass = (node->center_of_mass * old_total_charge + 
                                    fPositions[particle_index] * charge_to_add) / node->total_charge; 
        } else { 
            node->center_of_mass = fPositions[particle_index]; 
        } 
    } else { 
        node->center_of_mass = (min_bounds + max_bounds) * 0.5; 
    }
    
    // --- Handle leaf vs. internal node logic ---
    if (node->is_leaf) {
        if (node->particle_index == -1) { 
            node->particle_index = particle_index; 
        }
        else {
            int old_particle_index = node->particle_index; 
            bool aggregate = false;
            
            if (old_particle_index == -2) { 
                aggregate = true; 
            } else if (old_particle_index >= 0) { 
                if (fPositions[particle_index] == fPositions[old_particle_index]) { 
                    aggregate = true; 
                } else { 
                    const G4double minBoxSizeSq = (1.0 * CLHEP::nm) * (1.0 * CLHEP::nm); 
                    if ((max_bounds - min_bounds).mag2() < minBoxSizeSq) { 
                        aggregate = true; 
                    } 
                } 
            }
            
            if (aggregate && old_particle_index != -2) { 
                node->particle_index = -2; 
                return; 
            }
            
            // Subdivide the leaf node
            node->is_leaf = false; 
            node->particle_index = -1; 
            G4ThreeVector center = (min_bounds + max_bounds) * 0.5;
            
            // Re-insert the old particle
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
            
            // Insert the new particle
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
    } else { 
        // Internal node: find the correct child and recurse
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
    const G4double softening_factor_sq = (1.0*CLHEP::nm)*(1.0*CLHEP::nm); 
    
    if (d2 < softening_factor_sq) {
        d2 = softening_factor_sq;
    }

    if (node->is_leaf) { 
        if (node->particle_index != -1) { 
            G4double inv_d3 = 1.0 / (std::sqrt(d2) * d2); 
            G4double scale = node->total_charge * k_electric * inv_d3; 
            return displacement * scale; 
        } else {
            return G4ThreeVector(0,0,0); 
        }
    }
    
    // Barnes-Hut approximation check
    double width = (node_max.x() - node_min.x()); 
    double distance = std::sqrt(d2); 
    
    if (width / distance < barnes_hut_theta_) { 
        // Approximate
        G4double inv_d3 = 1.0 / (distance * d2); 
        G4double scale = node->total_charge * k_electric * inv_d3; 
        return displacement * scale; 
    } else { 
        // Recurse
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

G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const 
{ 
    if (!charge_root_) return G4ThreeVector(0,0,0); 
    
    G4ThreeVector min_box, max_box; 
    calculateBoundingBox(min_box, max_box); 
    
    return computeFieldWithApproximation(point, charge_root_.get(), min_box, max_box); 
}

// =========================================================================
// FIELD MAP OCTREE LOGIC (Unaltered from your input)
// =========================================================================

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> AdaptiveSumRadialFieldMap::buildFromScratch() 
{
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
    
    // --- FIX: Proper atomic update for max_depth_reached_ ---
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

void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) 
{
    if (!node) return;
    
    // --- FIX: Proper atomic update for max_depth_reached_ ---
    int current_max = max_depth_reached_.load(std::memory_order_relaxed);
    while (depth > current_max) {
        if (max_depth_reached_.compare_exchange_weak(current_max, depth, std::memory_order_relaxed)) {
            break;
        }
    }
    
    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x()); 
        
        if (depth < max_depth_ && size > minStepSize_ && 
            hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2)) 
        {
            gradient_refinements_++; 
            node->is_leaf = false; 
            leaf_nodes_--;
            
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
            
            for (int i = 0; i < 8; ++i) { 
                if(node->children[i]) {
#pragma omp taskwait
                    refineMeshByGradient(node->children[i].get(), depth + 1); 
                } 
            }
            // #pragma omp taskwait // Removed taskwait as per your original code
        }
    } else { 
        for (int i = 0; i < 8; ++i) { 
            if(node->children[i]) {
#pragma omp taskwait
                refineMeshByGradient(node->children[i].get(), depth + 1); 
            } 
        }
        // #pragma omp taskwait // Removed taskwait as per your original code
    }
}

bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(
    const G4ThreeVector& center, 
    const G4ThreeVector& center_field [[maybe_unused]], 
    double sample_distance) const 
{
    if (fPositions.empty()) return false; 
    
    // Adjusted range slightly
    double actual_distance = std::min(std::max(sample_distance, 0.1 * CLHEP::nm), 1.0 * CLHEP::um); 
    
    const std::vector<G4ThreeVector> sample_points = { 
        center + G4ThreeVector(actual_distance, 0, 0), 
        center - G4ThreeVector(actual_distance, 0, 0), 
        center + G4ThreeVector(0, actual_distance, 0), 
        center - G4ThreeVector(0, actual_distance, 0), 
        center + G4ThreeVector(0, 0, actual_distance), 
        center - G4ThreeVector(0, 0, actual_distance) 
    }; 
    
    std::vector<double> field_magnitudes(6);
    
#pragma omp parallel for // Keep parallel for from original
    for (int i = 0; i < 6; ++i) { 
        field_magnitudes[i] = computeFieldFromCharges(sample_points[i]).mag(); 
    }
    
    if (actual_distance == 0) return false; 
    
    double inv_2d = 1.0 / (2.0 * actual_distance); 
    
    double gx = (field_magnitudes[0] - field_magnitudes[1]) * inv_2d; 
    double gy = (field_magnitudes[2] - field_magnitudes[3]) * inv_2d; 
    double gz = (field_magnitudes[4] - field_magnitudes[5]) * inv_2d; 
    
    double grad_sq = gx * gx + gy * gy + gz * gz;
    
    // --- FIX: Correct comparison assuming threshold is V/m^2 ---
    // The division by (1e6/m) seemed incorrect dimensionally.
    return grad_sq > (fieldGradThreshold_ * fieldGradThreshold_);
}

void AdaptiveSumRadialFieldMap::collectFinalLeaves(Node* node) 
{ 
    if (!node) return; 
    
    if (node->is_leaf) { 
        all_leaves_.push_back(node); 
    } else { 
        for (int i = 0; i < 8; ++i) { 
            if(node->children[i]) 
                collectFinalLeaves(node->children[i].get()); 
        } 
    } 
}

// =========================================================================
// FIELD EVALUATION (Unaltered from your input)
// =========================================================================

void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const 
{ 
    const G4ThreeVector r(point[0], point[1], point[2]); 
    G4ThreeVector E = evaluateField(r); 
    
    field[0] = 0; 
    field[1] = 0; 
    field[2] = 0; 
    field[3] = E.x(); 
    field[4] = E.y(); 
    field[5] = E.z(); 
}

G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point) const 
{ 
    if (!pointInside(worldMin_, worldMax_, point)) return G4ThreeVector(0,0,0); 
    if (!root_) return G4ThreeVector(0,0,0); 
    
    return evaluateFieldRecursive(point, root_.get()); 
}

G4ThreeVector AdaptiveSumRadialFieldMap::evaluateFieldRecursive(
    const G4ThreeVector& point, 
    const Node* node) const 
{
    if (!node) return G4ThreeVector(0,0,0);
    
    if (node->is_leaf) return node->precomputed_field;
    
    G4ThreeVector center = node->center;
    int child_idx = 0;
    
    if (point.x() >= center.x()) child_idx |= 1;
    if (point.y() >= center.y()) child_idx |= 2;
    if (point.z() >= center.z()) child_idx |= 4;
    
    if (node->children[child_idx]) {
        return evaluateFieldRecursive(point, node->children[child_idx].get());
    } else {
        // Return zero instead of G4Exception
#pragma omp critical (error_log)
        G4cerr << "Warning: Point in internal node missing child during eval." << G4endl;
        return G4ThreeVector(0,0,0);
    }
}

// =========================================================================
// UTILITY AND I/O
// =========================================================================

bool AdaptiveSumRadialFieldMap::pointInside(
    const G4ThreeVector& min, 
    const G4ThreeVector& max, 
    const G4ThreeVector& point) const 
{
    return (point.x() >= min.x() && point.x() <= max.x() &&
            point.y() >= min.y() && point.y() <= max.y() &&
            point.z() >= min.z() && point.z() <= max.z());
}

void AdaptiveSumRadialFieldMap::calculateBoundingBox(
    G4ThreeVector& min, 
    G4ThreeVector& max) const 
{
    min = worldMin_;
    max = worldMax_;
}

void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const 
{
    G4cout << "=== Adaptive Mesh Statistics ===" << G4endl;
    G4cout << "Total nodes in field map: " << total_nodes_ << G4endl;
    G4cout << "Leaf nodes in field map: " << leaf_nodes_ << G4endl;
    G4cout << "Max depth reached: " << max_depth_reached_ << G4endl;
    G4cout << "Gradient-based refinements: " << gradient_refinements_ << G4endl;
    G4cout << "=================================" << G4endl;
}

void AdaptiveSumRadialFieldMap::collectStatistics(const Node* node, int depth) 
{
    // This function is currently just a placeholder.
}

void AdaptiveSumRadialFieldMap::calculateChildBounds(
    const G4ThreeVector& min, 
    const G4ThreeVector& max,
    const G4ThreeVector& center, 
    int child_index,
    G4ThreeVector& child_min, 
    G4ThreeVector& child_max) const 
{
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

void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const 
{
    std::ofstream outfile(filename, std::ios::binary);
    
    if (!outfile.is_open()) {
        G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, ("Failed to open file: " + filename).c_str());
        return;
    }
    
    double world_bounds[6] = { 
        worldMin_.x(), worldMin_.y(), worldMin_.z(), 
        worldMax_.x(), worldMax_.y(), worldMax_.z() 
    };
    
    int mesh_parameters[5] = { 
        max_depth_, 
        static_cast<int>(minStepSize_ / CLHEP::um), 
        total_nodes_, 
        leaf_nodes_, 
        static_cast<int>(fStorage) 
    };
    
    outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));
    
    writeFieldPointsToFile(outfile, root_.get());
    
    int statistics[3] = { 
        gradient_refinements_, 
        max_depth_reached_, 
        static_cast<int>(fPositions.size()) 
    };
    
    outfile.write(reinterpret_cast<const char*>(statistics), sizeof(statistics));
    outfile.close();
    
    G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
}

void AdaptiveSumRadialFieldMap::writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const 
{
    if (!node) return;
    
    if (node->is_leaf) {
        double position[3] = { 
            node->center.x(), 
            node->center.y(), 
            node->center.z() 
        };
        double field_value[3] = { 
            node->precomputed_field.x(), 
            node->precomputed_field.y(), 
            node->precomputed_field.z() 
        };
        
        outfile.write(reinterpret_cast<const char*>(position), sizeof(position));
        outfile.write(reinterpret_cast<const char*>(field_value), sizeof(field_value));
    } else {
        for (const auto& child : node->children) {
            writeFieldPointsToFile(outfile, child.get());
        }
    }
}