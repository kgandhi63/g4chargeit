#include "AdaptiveSumRadialFieldMap.hh"
#include "G4Exception.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh" // Includes epsilon0 and eplus

#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <vector>
#include <functional> // Needed if used elsewhere
#include <cstdint>   // Needed if used elsewhere
#include <atomic>    // Needed for atomic counters
#include <fstream>
#include <iomanip> // For setting precision
#include <numeric> // For std::accumulate

// Define physical constants locally if needed, but they should come from G4PhysicalConstants
static const double epsilon0_SI = 8.8541878128e-12 * farad / meter; // F/m
static const G4double k_electric = 1.0 / (4.0 * CLHEP::pi * CLHEP::epsilon0);

// --- Helper Function for Field Statistics (Used in Constructor) ---
namespace {
    struct FieldStats {
        double min;
        double max;
        double mean;
        double std_dev;
        double median;
        double iqr;
        double q1;
        double q3;
        int n_filtered;
    };

    // Implements linear interpolation for quantiles
    FieldStats calculateFieldStats(std::vector<double>& data) {

        // We use a small epsilon for floating point comparison instead of exactly 0.0
        const double epsilon = 1e-12; 
        
        // 1. Filter the data to get only non-zero magnitudes using modern C++ algorithms
        std::vector<double> nonzero_data;
        nonzero_data.reserve(data.size()); // Pre-allocate memory for efficiency

        // Copy elements from 'data' to 'nonzero_data' if the element (x) is greater than epsilon
        // Requires <algorithm> for copy_if and <iterator> for back_inserter (assumed included)
        std::copy_if(data.begin(), data.end(), 
                     std::back_inserter(nonzero_data), 
                     [&](double x){ return x > epsilon; });
        
        // Use the filtered vector for all subsequent calculations
        size_t n = nonzero_data.size();

        // Initialize all members. Must now include q1, q3, and n_filtered.
        FieldStats stats = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0}; 

        stats.n_filtered = n; // Store the count of data points used

        // 2. Sort the data to find min, max, median, and quartiles
        // Sorting the filtered vector is safe and efficient
        std::sort(nonzero_data.begin(), nonzero_data.end());

        stats.min = nonzero_data.front();
        stats.max = nonzero_data.back();

        // Mean
        double sum = std::accumulate(nonzero_data.begin(), nonzero_data.end(), 0.0);
        stats.mean = sum / n;

        // Standard Deviation (Population Standard Deviation)
        double sq_sum = 0.0;
        for (double val : nonzero_data) {
            sq_sum += (val - stats.mean) * (val - stats.mean);
        }
        stats.std_dev = std::sqrt(sq_sum / n);

        // Median (Q2)
        if (n % 2 == 1) {
            stats.median = nonzero_data[n / 2];
        } else {
            stats.median = (nonzero_data[n / 2 - 1] + nonzero_data[n / 2]) / 2.0;
        }

        // Quartiles (Q1 and Q3) - using the common 'R-6' or 'linear interpolation' method
        auto find_quantile = [&](double p) {
            if (p < 0.0 || p > 1.0) return 0.0;
            // Index starts at 1 in common formulas, so pos is the index in the sorted array (1-based)
            double pos = (n - 1.0) * p + 1.0; 
            double index = std::floor(pos);
            double delta = pos - index;
            
            if (index < 1) return nonzero_data.front();
            if (index >= n) return nonzero_data.back();

            // data[index-1] is the element at the index position (0-based)
            return nonzero_data[static_cast<size_t>(index) - 1] * (1.0 - delta) + nonzero_data[static_cast<size_t>(index)] * delta;
        };

        // double q1 = find_quantile(0.25);
        // double q3 = find_quantile(0.75);
        stats.q1 = find_quantile(0.25);
        stats.q3 = find_quantile(0.75);
        stats.iqr = stats.q3 - stats.q1;

        return stats;
    }
} // end namespace

namespace {
    // This is the helper struct needed for the "binning" in ApplyChargeDissipation
    struct NodeChargeInfo {
        std::vector<int> pos_indices;
        std::vector<int> neg_indices;
    };
}



// --- CONSTRUCTOR (Unaltered Sequence from your input) ---
AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    std::vector<G4ThreeVector>& positions,
    std::vector<G4double>& charges,
    const G4double& gradThreshold,
    const G4double& minStep,
    const G4double& time_step_dt,
    const G4double& material_temp_K,
    const std::string& state_filename,
    const std::string& filename,
    const G4ThreeVector& min_bounds, // Renamed from 'min'
    const G4ThreeVector& max_bounds, // Renamed from 'max'
    int max_depth_param,        // Renamed from 'max_depth'
    int initial_depth,
    bool dissipateCharge,       
    StorageType storage)
    : max_depth_(max_depth_param), minStepSize_(minStep),
      worldMin_(min_bounds), worldMax_(max_bounds), fieldGradThreshold_(gradThreshold), fStorage(storage),dissipateCharge_(dissipateCharge),
      fPositions(positions), fCharges(charges), fStateFilename(state_filename), initialDepth_(initial_depth) // Use references directly
{

    LoadPersistentState(fStateFilename, fPositions, fCharges);

    // Use store for atomic initialization
    total_nodes_.store(0);
    leaf_nodes_.store(0);
    gradient_refinements_.store(0);
    max_depth_reached_.store(0); // Use atomic for max_depth_reached_
    barnes_hut_theta_ = 0.5; // Set default Barnes-Hut theta

    G4cout << "Building initial charge octree..." << G4endl;
    buildChargeOctree(); // Based on original charges

    G4cout << "Building initial field octree..." << G4endl;
    all_leaves_.clear(); // Start with empty list
    root_ = buildFromScratch(); // Populates all_leaves_ with initial leaves

    // --- NEW SECTION: Calculate and Print Field Statistics for Initial Leaves ---
    std::vector<double> field_magnitudes;
    size_t num_leaves = all_leaves_.size(); 
    
    // 1. Resize the vector to the exact size needed
    field_magnitudes.resize(num_leaves); 

    G4cout << "Computing field values at initial leaf node (" << num_leaves << ")..." << G4endl;
    
    #pragma omp parallel for schedule(dynamic)
    // 2. Assign to the *pre-allocated* index 'i'
    for (size_t i = 0; i < num_leaves; ++i) { 
        Node* leaf = all_leaves_[i];
        if(leaf) {
            leaf->precomputed_field = computeFieldFromCharges(leaf->center); // Use NEW BH tree

            // Thread-safe assignment to an existing memory location
            field_magnitudes[i] = leaf->precomputed_field.mag(); 
        } else {
             // Handle null leaf if necessary (e.g., set to zero)
             field_magnitudes[i] = 0.0;
        }
    }

    // --------------------------------------------------------------------------
    FieldStats stats = calculateFieldStats(field_magnitudes);
    
    G4cout << "   >>> Mean of Field Magnitude: " << G4BestUnit(stats.mean,"Electric field") << " <<<" << G4endl;
    G4cout << "   >>> Median of Field Magnitude: " << G4BestUnit(stats.median,"Electric field") << " <<<" << G4endl;
    G4cout << "   >>> Std of Field Magnitude: " << G4BestUnit(stats.std_dev,"Electric field") << " <<<" << G4endl;
    G4cout << "   >>> IQR of Field Magnitude: " << G4BestUnit(stats.iqr,"Electric field") << " <<<" << G4endl;
    G4cout << "   >>> Min of Field Magnitude: " << G4BestUnit(stats.min,"Electric field") << " <<<" << G4endl;
    G4cout << "   >>> Max of Field Magnitude: " << G4BestUnit(stats.max,"Electric field") << " <<<" << G4endl;

    // reset the gradient threshold
    fieldGradThreshold_ = stats.max*gradThreshold; //iqr*1.5+stats.q3;
    G4cout << "   --> Field Gradient Threshold (V/m): " << G4BestUnit(fieldGradThreshold_,"Electric field") << G4endl;
    
    // --------------------------------------------------------------------------

    G4cout << "Refining field map based on field gradients..." << G4endl; // Log clarification
    #pragma omp parallel
    {
        #pragma omp single
        refineMeshByGradient(root_.get(), 0); // Refines based on fields from dissipated charges
    }

    if (dissipateCharge_) { 

        // --- Apply charge dissipation based on INITIAL mesh structure ---
        G4cout << "Applying one-time charge dissipation ('tax')..." << G4endl;
        ApplyChargeDissipation(time_step_dt, material_temp_K); // CALLS THE *NEW* DISSIPATION CODE
        // The master charge list (fCharges) has now been MODIFIED.
    } 

    // --- Collect FINAL leaves AFTER refinement (needed for export) ---
    all_leaves_.clear();
    collectFinalLeaves(root_.get()); // Collect final leaves
    leaf_nodes_.store(static_cast<int>(all_leaves_.size())); // Update count based on final leaves

    G4cout << "   --> Saving refined field to " <<filename << G4endl; // Log clarification
    if (!filename.empty()) {
        ExportFieldMapToFile(filename); // Export the final refined map
    }
    PrintMeshStatistics(); // Print final stats
    SaveFinalParticleState(state_filename);

}

// --- Destructor (If needed, likely empty with unique_ptr) ---
AdaptiveSumRadialFieldMap::~AdaptiveSumRadialFieldMap() {
    // std::unique_ptr handles memory automatically
}

void AdaptiveSumRadialFieldMap::LoadPersistentState(const std::string& filename,
                                                    std::vector<G4ThreeVector>& positions,
                                                    std::vector<G4double>& charges)
{
    size_t initial_count = positions.size();
    G4cout << "Loading persistent particle state from " << filename << " and appending..." << G4endl;
    std::ifstream infile(filename, std::ios::binary);

    if (infile.is_open()) {
        uint64_t num_particles = 0;
        infile.read(reinterpret_cast<char*>(&num_particles), sizeof(uint64_t));

        for (uint64_t i = 0; i < num_particles; ++i) {
            double pos[3];
            double charge;
            infile.read(reinterpret_cast<char*>(pos), sizeof(pos));
            infile.read(reinterpret_cast<char*>(&charge), sizeof(double));

            if (infile.fail()) {
                G4cerr << "Warning: Unexpected EOF or read failure while reading particle " << i << G4endl;
                break;
            }

            positions.push_back(G4ThreeVector(pos[0], pos[1], pos[2]));
            charges.push_back(charge);
        }

        infile.close();
        G4cout << "   Loaded " << positions.size() - initial_count << " persistent particles." << G4endl;
        G4cout << "   Total particles now: " << positions.size() << G4endl;
    } else {
        G4cout << "   " << filename << " not found. Using only initially provided particles." << G4endl;
    }

    if (positions.size() != charges.size()) {
        G4Exception("AdaptiveSumRadialFieldMap::LoadPersistentState", "SizeMismatch", FatalException,
                    "Position and charge vectors have different sizes after loading state.");
    }
}

// --- ADDED: Save State ---// --- MODIFIED: Save State (Only Non-Zero Charges) ---
void AdaptiveSumRadialFieldMap::SaveFinalParticleState(const std::string& filename) const
{
    G4cout << "Saving final particle state (non-zero charges) to " << filename << "..." << G4endl;
    std::ofstream outfile(filename, std::ios::trunc | std::ios::binary); // Overwrite, binary format
    if (!outfile.is_open()) {
        G4cerr << "Error: Could not open " << filename << " for writing particle state!" << G4endl;
        return;
    }

    uint64_t num_particles_to_save = 0; // Counter for non-zero particles
    auto count_position = outfile.tellp(); // Remember where the count goes
    outfile.write(reinterpret_cast<const char*>(&num_particles_to_save), sizeof(uint64_t));

    // 2. Iterate and write only non-zero particles
    const G4double charge_threshold = 1e-21 * CLHEP::eplus; // Threshold to consider charge zero

    for (size_t i = 0; i < fPositions.size(); ++i) {
        // --- ADDED CHECK ---
        if (std::abs(fCharges[i]) > charge_threshold) {
            // Write position as doubles (in internal units, e.g., mm)
            double pos[3] = { fPositions[i].x(), fPositions[i].y(), fPositions[i].z() };
            outfile.write(reinterpret_cast<const char*>(pos), sizeof(pos));
            // Write charge as double (in internal units, e.g., e+)
            double charge = fCharges[i];
            outfile.write(reinterpret_cast<const char*>(&charge), sizeof(double));

            num_particles_to_save++; // Increment count of saved particles
        }
        // --- END CHECK ---
    }

    // 3. Go back and write the actual count
    outfile.seekp(count_position); // Move write pointer back to the beginning
    outfile.write(reinterpret_cast<const char*>(&num_particles_to_save), sizeof(uint64_t)); // Write the correct count
    outfile.seekp(0, std::ios::end); // Go back to the end if writing more data later (optional)


    outfile.close();
    if (outfile.good()) {
        G4cout << "   Particle state saved (" << num_particles_to_save << " non-zero particles)." << G4endl;
    } else {
         G4cerr << "Error: Could not write successfully to " << filename << G4endl;
    }
}


void AdaptiveSumRadialFieldMap::ApplyChargeDissipation(G4double dt_internal, G4double temp_K) {

    G4cout << "    Building particle-to-leaf map (parallel map-reduce)..." << G4endl;

    // --- STEP 1: Parallel "Map" Phase ---
    // We loop over all N particles in parallel. Each particle finds its own
    // leaf bin (O(log L)) and adds itself to a *thread-private* map.
    // This is the O(N log L) part, now fully parallelized.

    // This vector will hold one "private" map for each thread.
    int num_threads = 1;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }
    std::vector<std::map<Node*, NodeChargeInfo>> private_maps(num_threads);

    #pragma omp parallel
    {
        // Get this thread's ID (from 0 to num_threads-1)
        int thread_id = omp_get_thread_num();
        
        // Get the pre-allocated map for this specific thread
        auto& thread_local_map = private_maps[thread_id]; 

        // Split the main particle loop (N) across all threads
        #pragma omp for schedule(dynamic, 1000)
        for (size_t i = 0; i < fPositions.size(); ++i) {
            
            // Skip particles with effectively zero charge
            if (std::abs(fCharges[i]) < 1e-21 * CLHEP::eplus) continue;

            // Use the octree to find the particle's leaf node (the fast log L part)
            Node* leaf = findLeafNode(fPositions[i], root_.get()); 
            
            if (!leaf) continue; // Particle is outside the map, skip it.

            // Add the particle's index to this thread's private map
            if (fCharges[i] > 0) {
                thread_local_map[leaf].pos_indices.push_back(static_cast<int>(i));
            } else {
                thread_local_map[leaf].neg_indices.push_back(static_cast<int>(i));
            }
        }
    } // --- End of parallel "Map" region ---


    // --- STEP 2: Serial "Reduce" Phase ---
    // We are now back on the main thread.
    // Merge all the small private maps into one single global map.
    // This is serial, but it's very fast.
    
    G4cout << "    Merging " << private_maps.size() << " thread-local maps..." << G4endl;
    std::map<Node*, NodeChargeInfo> global_leaf_map; 

    for (const auto& local_map : private_maps) { // Loop over each thread's map
        for (const auto& pair : local_map) {     // Loop over each leaf in that map
            Node* leaf = pair.first;
            const NodeChargeInfo& info = pair.second;

            // Append the lists of indices from the private map to the global map
            global_leaf_map[leaf].pos_indices.insert(
                global_leaf_map[leaf].pos_indices.end(),
                info.pos_indices.begin(), info.pos_indices.end()
            );
            global_leaf_map[leaf].neg_indices.insert(
                global_leaf_map[leaf].neg_indices.end(),
                info.neg_indices.begin(), info.neg_indices.end()
            );
        }
    }
    
    private_maps.clear(); // Free the memory from the temporary maps

    // 'global_leaf_map' now contains ONLY the leaves that
    // actually have particles in them, and their complete particle lists.

    // --- STEP 3: Parallel "Apply" Phase ---
    // Now we loop over the much smaller 'global_leaf_map' (K leaves)
    // and apply the physics, using the logic from your function.
    
    G4cout << "    Applying dissipation to " << global_leaf_map.size() << " active leaves..." << G4endl;

    // --- Setup for dissipation (from your code) ---
    double conductivity_SI = calculateConductivity(temp_K);
    const double epsilon0_SI_Value = CLHEP::epsilon0 / (farad/meter); // Get SI value
    double rate_constant_Value = 0.0;
    if (epsilon0_SI_Value > 1e-18) { // Avoid division by zero
        rate_constant_Value = conductivity_SI / epsilon0_SI_Value;
    } else {
        G4cerr << "Warning: Epsilon0 value is too small, dissipation rate set to 0." << G4endl;
    }

    G4double total_dissipated_charge = 0.0;

    // Convert map to a vector to iterate in parallel safely
    std::vector<std::pair<Node*, NodeChargeInfo>> leaf_vector(global_leaf_map.begin(), global_leaf_map.end());

    #pragma omp parallel for schedule(dynamic) reduction(+:total_dissipated_charge)
    for (size_t i = 0; i < leaf_vector.size(); ++i) {
        
        const auto& pair = leaf_vector[i];
        // const Node* leaf_node = pair.first; // We don't need the leaf geometry anymore
        
        // Get the particle lists for this leaf (these are already built!)
        const std::vector<int>& positive_particle_indices = pair.second.pos_indices;
        const std::vector<int>& negative_particle_indices = pair.second.neg_indices;

        // --- This replaces your O(N) inner loop ---
        // We just loop over the small lists we already built
        double Q_node_net_Internal = 0.0;
        for (int idx : positive_particle_indices) {
             if (idx >= 0 && static_cast<size_t>(idx) < fCharges.size()) Q_node_net_Internal += fCharges[idx];
        }
        for (int idx : negative_particle_indices) {
             if (idx >= 0 && static_cast<size_t>(idx) < fCharges.size()) Q_node_net_Internal += fCharges[idx];
        }
        // --- End of replacement logic ---

        if (std::abs(Q_node_net_Internal) < 1e-21 * CLHEP::eplus) continue;

        // --- Strip units explicitly before calculation (from your code) ---
        double Q_node_net_Value = Q_node_net_Internal / CLHEP::eplus; // Dimensionless number of elementary charges
        
        // --- FIX: Ensure dt_internal is converted to seconds value ---
        // Your previous code had `double dt_Value = dt_internal;`
        // This assumes dt_internal was already a simple 'double'.
        // This version from your *original* (correct) code is safer:
        double dt_Value = dt_internal / CLHEP::second; // Make sure dt_internal has time unit!
        
        double delta_Q_node_Value = -rate_constant_Value * Q_node_net_Value * dt_Value;

        // --- Re-apply the unit at the end ---
        double delta_Q_node = delta_Q_node_Value * CLHEP::eplus; // Result now has units of e+
        
        double charge_change_magnitude = std::abs(delta_Q_node); // Still in e+ units

        //G4cerr << "charge change mag " << charge_change_magnitude << G4endl;

        if (charge_change_magnitude > 1e-14 * CLHEP::eplus) { // Use a small threshold
            total_dissipated_charge += charge_change_magnitude; // Accumulate magnitude in e+

            // distributeChargeChange is already thread-safe (uses #omp atomic)
            if (delta_Q_node < 0 && !positive_particle_indices.empty()) {
                distributeChargeChange(positive_particle_indices, -charge_change_magnitude);
            } else if (delta_Q_node > 0 && !negative_particle_indices.empty()) {
                distributeChargeChange(negative_particle_indices, charge_change_magnitude);
            }
        }
    } // End of parallel "Apply" loop

    G4cout << "    --> Charge dissipation applied (neutralizing)." << G4endl;
    G4cout << "    >>> Total charge dissipated/neutralized this step: "
           << total_dissipated_charge / CLHEP::eplus << " e <<<" << G4endl;
}

// --- calculateConductivity remains the same as your input ---
double AdaptiveSumRadialFieldMap::calculateConductivity(double temp_K) const {
    if (temp_K <= 0) return 0.0;
    // Formula from user's source image
    return 6.0e-18 * std::exp(0.0230 * temp_K);
}

// --- NEW UNIFIED HELPER FUNCTION: distributeChargeChange ---
// Replaces the old removeChargeFromRegion function
void AdaptiveSumRadialFieldMap::distributeChargeChange(const std::vector<int>& particle_indices, G4double total_charge_change) {
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
             if ((charge_change_per_particle < 0 && current_charge < 1e-21 * CLHEP::eplus) || // Went negative
                 (charge_change_per_particle > 0 && current_charge > -1e-21 * CLHEP::eplus)) { // Went positive
                  #pragma omp atomic write
                  fCharges[idx] = 0.0;
             }
         } else {
            #pragma omp critical (error_log) // Use critical section for thread-safe error output
            G4cerr << "Warning: Invalid particle index (" << idx << ") in distributeChargeChange." << G4endl;
         }
     }
}

void AdaptiveSumRadialFieldMap::buildChargeOctree() {
    charge_root_.reset(); // Important when rebuilding
    if (fPositions.empty()) return;
    G4ThreeVector min_box, max_box; calculateBoundingBox(min_box, max_box);
    charge_root_ = std::make_unique<ChargeNode>();
    charge_root_->is_leaf = true; charge_root_->particle_index = -1; charge_root_->total_charge = 0.0; charge_root_->center_of_mass = (min_box + max_box) * 0.5;
    for (size_t i = 0; i < fPositions.size(); ++i) {
        if (std::abs(fCharges[i]) > 1e-25 * CLHEP::coulomb && pointInside(min_box, max_box, fPositions[i])) {
            insertCharge(charge_root_.get(), static_cast<int>(i), min_box, max_box);
        }
    }
}
void AdaptiveSumRadialFieldMap::insertCharge(ChargeNode* node, int particle_index, const G4ThreeVector& min_bounds, const G4ThreeVector& max_bounds) {
    // *** IMPORTANT: Fill in the G4Exception call properly ***
    if (!node) return; if (particle_index < 0 || static_cast<size_t>(particle_index) >= fPositions.size()) {
        G4Exception("AdaptiveSumRadialFieldMap::insertCharge", "IndexOutOfBounds", FatalException, "Invalid particle index provided."); return;
    }
    G4double old_total_charge = node->total_charge; G4double charge_to_add = fCharges[particle_index]; node->total_charge += charge_to_add;
    if (std::abs(node->total_charge) > 1e-25 * CLHEP::coulomb) { if (std::abs(old_total_charge) > 1e-25 * CLHEP::coulomb) { node->center_of_mass = (node->center_of_mass * old_total_charge + fPositions[particle_index] * charge_to_add) / node->total_charge; } else { node->center_of_mass = fPositions[particle_index]; } } else { node->center_of_mass = (min_bounds + max_bounds) * 0.5; }
    if (node->is_leaf) {
        if (node->particle_index == -1) { node->particle_index = particle_index; }
        else {
            int old_particle_index = node->particle_index; bool aggregate = false;
            if (old_particle_index == -2) { aggregate = true; } else if (old_particle_index >= 0) { if (fPositions[particle_index] == fPositions[old_particle_index]) { aggregate = true; } else { const G4double minBoxSizeSq = (1.0 * nm) * (1.0 * nm); if ((max_bounds - min_bounds).mag2() < minBoxSizeSq) { aggregate = true; } } }
            if (aggregate && old_particle_index != -2) { node->particle_index = -2; return; }
            node->is_leaf = false; node->particle_index = -1; G4ThreeVector center = (min_bounds + max_bounds) * 0.5;
            if (old_particle_index >= 0) {
                 int old_child_idx = 0; if (fPositions[old_particle_index].x() >= center.x()) old_child_idx |= 1; if (fPositions[old_particle_index].y() >= center.y()) old_child_idx |= 2; if (fPositions[old_particle_index].z() >= center.z()) old_child_idx |= 4;
                 G4ThreeVector old_min, old_max; calculateChildBounds(min_bounds, max_bounds, center, old_child_idx, old_min, old_max); if (!node->children[old_child_idx]) node->children[old_child_idx] = std::make_unique<ChargeNode>(); insertCharge(node->children[old_child_idx].get(), old_particle_index, old_min, old_max);
            }
             int new_child_idx = 0; if (fPositions[particle_index].x() >= center.x()) new_child_idx |= 1; if (fPositions[particle_index].y() >= center.y()) new_child_idx |= 2; if (fPositions[particle_index].z() >= center.z()) new_child_idx |= 4;
             G4ThreeVector new_min, new_max; calculateChildBounds(min_bounds, max_bounds, center, new_child_idx, new_min, new_max); if (!node->children[new_child_idx]) node->children[new_child_idx] = std::make_unique<ChargeNode>(); insertCharge(node->children[new_child_idx].get(), particle_index, new_min, new_max);
        }
    } else { G4ThreeVector center = (min_bounds + max_bounds) * 0.5; int child_idx = 0; if (fPositions[particle_index].x() >= center.x()) child_idx |= 1; if (fPositions[particle_index].y() >= center.y()) child_idx |= 2; if (fPositions[particle_index].z() >= center.z()) child_idx |= 4; G4ThreeVector c_min, c_max; calculateChildBounds(min_bounds, max_bounds, center, child_idx, c_min, c_max); if (!node->children[child_idx]) node->children[child_idx] = std::make_unique<ChargeNode>(); insertCharge(node->children[child_idx].get(), particle_index, c_min, c_max); }
}



G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldWithApproximation(const G4ThreeVector& point, const ChargeNode* node, const G4ThreeVector& node_min, const G4ThreeVector& node_max) const {
    if (!node || std::abs(node->total_charge) < 1e-25 * CLHEP::coulomb) return G4ThreeVector(0,0,0); G4ThreeVector displacement = point - node->center_of_mass; G4double d2 = displacement.mag2(); const G4double softening_factor_sq = (1.0*nm)*(1.0*nm); if (d2 < softening_factor_sq) d2 = softening_factor_sq;
    if (node->is_leaf) { if (node->particle_index != -1) { G4double inv_d3 = 1.0 / (std::sqrt(d2) * d2); G4double scale = node->total_charge * k_electric * inv_d3; return displacement * scale; } else return G4ThreeVector(0,0,0); }
    double width = (node_max.x() - node_min.x()); double distance = std::sqrt(d2); if (width / distance < barnes_hut_theta_) { G4double inv_d3 = 1.0 / (distance * d2); G4double scale = node->total_charge * k_electric * inv_d3; return displacement * scale; }
    else { G4ThreeVector E_total(0,0,0); G4ThreeVector center = (node_min + node_max) * 0.5; for(int i = 0; i < 8; ++i) { if(node->children[i]) { G4ThreeVector c_min, c_max; calculateChildBounds(node_min, node_max, center, i, c_min, c_max); E_total += computeFieldWithApproximation(point, node->children[i].get(), c_min, c_max); } } return E_total; }
}
G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const { if (!charge_root_) return G4ThreeVector(0,0,0); G4ThreeVector min_box, max_box; calculateBoundingBox(min_box, max_box); return computeFieldWithApproximation(point, charge_root_.get(), min_box, max_box); }

// --- FIELD MAP OCTREE LOGIC (Unaltered from your input) ---

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> AdaptiveSumRadialFieldMap::buildFromScratch() {
    
    G4cout << "   Building coarse grid to depth " << initialDepth_ << "..." << G4endl;

    G4ThreeVector min_box, max_box;
    calculateBoundingBox(min_box, max_box);
    auto root_node = std::make_unique<Node>();
    root_node->min = min_box;
    root_node->max = max_box;
    root_node->center = (min_box + max_box) * 0.5;

    total_nodes_.store(1); // Start with 1 (the root)
    max_depth_reached_.store(0);
    
    // Call the new helper to build the uniform grid
    buildUniformGrid(root_node.get(), 0);

    // After building, collect all the leaves of this coarse grid
    all_leaves_.clear();
    collectFinalLeaves(root_node.get());
    leaf_nodes_.store(static_cast<int>(all_leaves_.size()));
    
    G4cout << "   Coarse grid built with " << leaf_nodes_.load() << " leaf nodes." << G4endl;

    return root_node;
}

// --- ADD THIS NEW HELPER FUNCTION ---

void AdaptiveSumRadialFieldMap::buildUniformGrid(Node* node, int depth)
{
    // Base Case: We've reached the target initial depth. Mark as leaf and stop.
    if (depth == initialDepth_) {
        node->is_leaf = true;
        return;
    }

    // Recursive Case: Not deep enough. Subdivide.
    node->is_leaf = false;
    for (int i = 0; i < 8; ++i) {
        G4ThreeVector child_min, child_max;
        calculateChildBounds(node->min, node->max, node->center, i, child_min, child_max);

        auto child = std::make_unique<Node>();
        child->min = child_min;
        child->max = child_max;
        child->center = (child_min + child_max) * 0.5;

        total_nodes_++; // Atomically increment total node count

        // Recursively call for the next depth
        buildUniformGrid(child.get(), depth + 1);

        node->children[i] = std::move(child);
    }
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> AdaptiveSumRadialFieldMap::createOctreeFromScratch(const G4ThreeVector& min_bounds, const G4ThreeVector& max_bounds, int depth) {
    auto node = std::make_unique<Node>(); node->min = min_bounds; node->max = max_bounds; node->center = (min_bounds + max_bounds) * 0.5; node->is_leaf = true; total_nodes_++;
    // --- FIX: Proper atomic update for max_depth_reached_ ---
    int current_max = max_depth_reached_.load(std::memory_order_relaxed);
    while (depth > current_max) {
        if (max_depth_reached_.compare_exchange_weak(current_max, depth, std::memory_order_relaxed)) break;
        // If exchange failed, current_max was updated, loop again
    }
    leaf_nodes_++; return node;
}


AdaptiveSumRadialFieldMap::Node* AdaptiveSumRadialFieldMap::findLeafNode(const G4ThreeVector& point, Node* node) const
{
    if (!node) return nullptr;
    if (node->is_leaf) return node; // Found the leaf

    G4ThreeVector center = node->center;
    int child_idx = 0;
    if (point.x() >= center.x()) child_idx |= 1;
    if (point.y() >= center.y()) child_idx |= 2;
    if (point.z() >= center.z()) child_idx |= 4;

    if (node->children[child_idx]) {
        // Recurse into the correct child
        return findLeafNode(point, node->children[child_idx].get());
    }

    // Point is in an internal node but the child doesn't exist
    // This might indicate a logic error elsewhere if the point is within bounds.
    // Returning nullptr is safer than crashing.
    #pragma omp critical (error_log)
    { // Add braces for proper critical section scope
        G4cerr << "Warning: findLeafNode reached internal node with missing child for point "
               << point << " within node bounds [" << node->min << ", " << node->max << "]" << G4endl;
    }
    return nullptr;
}

// --- REPLACE your current refineMeshByGradient WITH THIS ---

void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) {
    if (!node) return;

    // Atomically update max depth
    int current_max = max_depth_reached_.load(std::memory_order_relaxed);
    while (depth > current_max) {
        if (max_depth_reached_.compare_exchange_weak(current_max, depth, std::memory_order_relaxed)) break;
    }

    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x());
        // Check if refinement is needed
        if (depth < max_depth_ && size > minStepSize_ && hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2)) {

            gradient_refinements_++;
            node->is_leaf = false;
            leaf_nodes_--; // This leaf is no longer a leaf

            // Create 8 new children
            for (int i = 0; i < 8; ++i) {
                G4ThreeVector c_min, c_max;
                calculateChildBounds(node->min, node->max, node->center, i, c_min, c_max);
                auto child = std::make_unique<Node>();
                child->min = c_min;
                child->max = c_max;
                child->center = (c_min + c_max) * 0.5;
                child->is_leaf = true; // The new children are leaves
                child->precomputed_field = computeFieldFromCharges(child->center);
                total_nodes_++;
                leaf_nodes_++; // Add new leaf
                node->children[i] = std::move(child);
            }

            // Create new tasks to refine the children
            for (int i = 0; i < 8; ++i) {
                if(node->children[i]) {
                    #pragma omp task
                    refineMeshByGradient(node->children[i].get(), depth + 1);
                }
            }
            // --- THIS IS THE FIX ---
            // Wait for all child tasks to complete *before* this function returns
            #pragma omp taskwait
        }
        // else: This leaf does not need refinement, so just return.
    } else {
        // This is an internal node, create tasks for its children
        for (int i = 0; i < 8; ++i) {
            if(node->children[i]) {
                #pragma omp task
                refineMeshByGradient(node->children[i].get(), depth + 1);
            }
        }
        // --- THIS IS THE FIX ---
        // Wait for all child tasks to complete
        #pragma omp taskwait
    }
}


bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field [[maybe_unused]], double sample_distance) const {
    if (fPositions.empty()) return false; double actual_distance = std::min(std::max(sample_distance, 0.1 * nm), 1.0 * um); // Adjusted range slightly
    const std::vector<G4ThreeVector> sample_points = { center + G4ThreeVector(actual_distance, 0, 0), center - G4ThreeVector(actual_distance, 0, 0), center + G4ThreeVector(0, actual_distance, 0), center - G4ThreeVector(0, actual_distance, 0), center + G4ThreeVector(0, 0, actual_distance), center - G4ThreeVector(0, 0, actual_distance) }; std::vector<double> field_magnitudes(6);
    #pragma omp parallel for // Keep parallel for from original
    for (int i = 0; i < 6; ++i) { field_magnitudes[i] = computeFieldFromCharges(sample_points[i]).mag(); }
    if (actual_distance == 0) return false; double inv_2d = 1.0/(2.0*actual_distance); double gx=(field_magnitudes[0]-field_magnitudes[1])*inv_2d; double gy=(field_magnitudes[2]-field_magnitudes[3])*inv_2d; double gz=(field_magnitudes[4]-field_magnitudes[5])*inv_2d; double grad_sq = gx*gx+gy*gy+gz*gz;
    // --- FIX: Correct comparison assuming threshold is V/m^2 ---
    // The division by (1e6/m) seemed incorrect dimensionally.
    return grad_sq > (fieldGradThreshold_*fieldGradThreshold_);
}
void AdaptiveSumRadialFieldMap::collectFinalLeaves(Node* node) { if (!node) return; if (node->is_leaf) { all_leaves_.push_back(node); } else { for (int i = 0; i < 8; ++i) { if(node->children[i]) collectFinalLeaves(node->children[i].get()); } } }

// --- FIELD EVALUATION (Unaltered from your input) ---
void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const { const G4ThreeVector r(point[0], point[1], point[2]); G4ThreeVector E = evaluateField(r); field[0]=0; field[1]=0; field[2]=0; field[3]=E.x(); field[4]=E.y(); field[5]=E.z(); }
G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point) const { if (!pointInside(worldMin_, worldMax_, point)) return G4ThreeVector(0,0,0); if (!root_) return G4ThreeVector(0,0,0); return evaluateFieldRecursive(point, root_.get()); }
G4ThreeVector AdaptiveSumRadialFieldMap::evaluateFieldRecursive(const G4ThreeVector& point, const Node* node) const {
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

// --- UTILITY AND I/O (Unaltered from your input) ---
bool AdaptiveSumRadialFieldMap::pointInside(const G4ThreeVector& min_bounds, const G4ThreeVector& max_bounds, const G4ThreeVector& point) const { return (point.x() >= min_bounds.x() && point.x() <= max_bounds.x() && point.y() >= min_bounds.y() && point.y() <= max_bounds.y() && point.z() >= min_bounds.z() && point.z() <= max_bounds.z()); }
void AdaptiveSumRadialFieldMap::calculateBoundingBox(G4ThreeVector& min_box, G4ThreeVector& max_box) const { min_box = worldMin_; max_box = worldMax_; }
void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const { G4cout << "\n=== Adaptive Mesh Statistics ===" << G4endl; G4cout << "Total nodes created:      " << total_nodes_.load() << G4endl; G4cout << "Final leaf nodes:         " << leaf_nodes_.load() << G4endl; G4cout << "Max octree depth reached: " << max_depth_reached_.load() << G4endl; G4cout << "Gradient refinements:     " << gradient_refinements_.load() << G4endl; G4cout << "=================================\n" << G4endl; }
void AdaptiveSumRadialFieldMap::calculateChildBounds(const G4ThreeVector& p_min, const G4ThreeVector& p_max, const G4ThreeVector& p_cen, int c_idx, G4ThreeVector& c_min, G4ThreeVector& c_max) const { c_min.setX((c_idx & 1) ? p_cen.x() : p_min.x()); c_min.setY((c_idx & 2) ? p_cen.y() : p_min.y()); c_min.setZ((c_idx & 4) ? p_cen.z() : p_min.z()); c_max.setX((c_idx & 1) ? p_max.x() : p_cen.x()); c_max.setY((c_idx & 2) ? p_max.y() : p_cen.y()); c_max.setZ((c_idx & 4) ? p_max.z() : p_cen.z()); }

// --- MODIFIED Export Function: Removed particle positions and charges ---
void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
    G4cout << "Exporting adaptive binary field map (all " << total_nodes_.load() << " nodes)..." << G4endl;

    std::ofstream outfile(filename, std::ios::binary | std::ios::trunc);
    if (!outfile.is_open()) {
        G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile", "FileOpenError", FatalException, ("Failed to open file for writing: " + filename).c_str());
        return;
    }

    // Bounds
    // double world_bounds[6] = { worldMin_.x(), worldMin_.y(), worldMin_.z(), worldMax_.x(), worldMax_.y(), worldMax_.z() };
    // outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));

    // Octree Parameters
    uint32_t max_d = static_cast<uint32_t>(max_depth_);
    double min_s = minStepSize_;
    //uint32_t storage_type = static_cast<uint32_t>(fStorage);
    // These two are the counts you are interested in
    uint64_t total_node_count = static_cast<uint64_t>(total_nodes_.load());
    uint64_t final_leaf_count = static_cast<uint64_t>(leaf_nodes_.load()); 

    outfile.write(reinterpret_cast<const char*>(&max_d), sizeof(max_d));
    outfile.write(reinterpret_cast<const char*>(&min_s), sizeof(min_s));
    //outfile.write(reinterpret_cast<const char*>(&storage_type), sizeof(storage_type));
    outfile.write(reinterpret_cast<const char*>(&total_node_count), sizeof(total_node_count));
    outfile.write(reinterpret_cast<const char*>(&final_leaf_count), sizeof(final_leaf_count)); 

    // 2. Write all nodes recursively (leaves and internal nodes)
    writeFieldPointsToFileRecursive(outfile, root_.get()); 
    
    // 3. Write Final Statistics (unchanged)
    uint32_t grad_ref = static_cast<uint32_t>(gradient_refinements_.load());
    uint32_t max_depth_r = static_cast<uint32_t>(max_depth_reached_.load());
    outfile.write(reinterpret_cast<const char*>(&grad_ref), sizeof(grad_ref));
    outfile.write(reinterpret_cast<const char*>(&max_depth_r), sizeof(max_depth_r));

    outfile.close();
    //G4cout << "Adaptive binary field map exported (Total Nodes: " << total_node_count << ")." << G4endl;
}


void AdaptiveSumRadialFieldMap::writeFieldPointsToFileRecursive(std::ofstream& outfile, const Node* node) const {
    if (!node) return;

    // --- Write Node Data ---

    // Pos/Center
    float pos[3] = { static_cast<float>(node->center.x()), static_cast<float>(node->center.y()), static_cast<float>(node->center.z()) };
    outfile.write(reinterpret_cast<const char*>(pos), sizeof(pos));
    
    // Field (only precomputed for leaves, zero for internal nodes if not computed)
    float field[3] = { static_cast<float>(node->precomputed_field.x()), static_cast<float>(node->precomputed_field.y()), static_cast<float>(node->precomputed_field.z()) };
    outfile.write(reinterpret_cast<const char*>(field), sizeof(field));
    
    // // Size and Type Flag
    // float size = static_cast<float>(node->max.x() - node->min.x());
    // outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // // Write a flag to indicate if it's a leaf (1) or an internal node (0)
    // uint8_t is_leaf_flag = node->is_leaf ? 1 : 0;
    // outfile.write(reinterpret_cast<const char*>(&is_leaf_flag), sizeof(is_leaf_flag));
    
    // --- Recurse on Children ---
    if (!node->is_leaf) {
        for (int i = 0; i < 8; ++i) {
            writeFieldPointsToFileRecursive(outfile, node->children[i].get());
        }
    }
}
