#include "AdaptiveSumRadialFieldMap.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Exception.hh"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <stdexcept>
#include <immintrin.h>
#include <iomanip>

AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    const std::vector<G4ThreeVector>& positions,  // Add charge positions
    const std::vector<G4double>& charges,         // Add charges
    const G4double& gradThreshold,
    const G4double& minStep,
    const std::string& filename,
    const G4ThreeVector& min,
    const G4ThreeVector& max,
    int max_depth,
    StorageType storage)
    : max_depth_(max_depth), minStepSize_(minStep),
      worldMin_(min), worldMax_(max), fieldGradThreshold_(gradThreshold), fStorage(storage),
      fPositions(positions), fCharges(charges), was_gradient_refined_(false)  // Initialize charge data
{
    // Initialize statistics
    total_nodes_ = 0;
    leaf_nodes_ = 0;
    gradient_refinements_ = 0;
    max_depth_reached_ = 0;

    G4cout << "=== AdaptiveSumRadialFieldMap Constructor ===" << G4endl;
    G4cout << "  World bounds: [" << min.x()/um << ", " << min.y()/um << ", " << min.z()/um << "] um"
           << " to [" << max.x()/um << ", " << max.y()/um << ", " << max.z()/um << "] um" << G4endl;
    G4cout << "  minStepSize: " << minStepSize_/um << " um" << G4endl;
    G4cout << "  max_depth: " << max_depth_ << G4endl;
    G4cout << "  fieldGradThreshold: " << fieldGradThreshold_ << " V/m" << G4endl;
    G4cout << "  Number of charges: " << fPositions.size() << G4endl;
    
    // Verify charges are available
    if (fPositions.empty()) {
        G4cout << "WARNING: No charge positions provided!" << G4endl;
    } 

    // Set world dimensions
    setWorldDimensions(worldMax_.x() - worldMin_.x(), worldMax_.y() - worldMin_.y(), worldMax_.z() - worldMin_.z());

    // CONSTRUCTOR FLOW VERIFICATION
    G4cout << "=== CONSTRUCTOR FLOW VERIFICATION ===" << G4endl;

    // Step 1: Build initial octree
    G4cout << "Step 1: Building initial octree..." << G4endl;
    root_ = buildFromScratch();
    G4cout << "Initial octree built. Total leaves: " << leaf_nodes_ << G4endl;

    // Step 2: Refine based on gradients  
    G4cout << "Step 2: Starting gradient-based refinement..." << G4endl;
    G4cout << "Before refinement - gradient_refinements_: " << gradient_refinements_ << G4endl;

    #pragma omp parallel
    {
        #pragma omp single
        refineMeshByGradient(root_.get(), 0);
    }

    G4cout << "After refinement - gradient_refinements_: " << gradient_refinements_ << G4endl;
    G4cout << "=== END CONSTRUCTOR FLOW ===" << G4endl;

    // Collect final statistics
    collectStatistics(root_.get(), 0);

    // Export if filename provided
    if (!filename.empty()) {
        ExportFieldMapToFile(filename);
    }
    
    PrintMeshStatistics();
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildFromScratch() {
    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    
    // Create initial octree
    G4cout << "Creating initial octree from scratch..." << G4endl;
    auto root = createOctreeFromScratch(min, max, 0);
    
    // Refine based on field gradients
    G4cout << "Refining mesh based on field gradients..." << G4endl;
    refineMeshByGradient(root_.get(), 0);
    
    return root;
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth) {
    static int node_count = 0;
    node_count++;

    // G4cout << "=== createOctreeFromScratch call #" << node_count 
    //        << ", depth: " << depth << " ===" << G4endl;
    // G4cout << " Bounds: [" << min.x()/um << ", " << min.y()/um << ", " << min.z()/um << "] um"
    //        << " to [" << max.x()/um << ", " << max.y()/um << ", " << max.z()/um << "] um" << G4endl;

    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;

    #pragma omp atomic
    total_nodes_++;

    #pragma omp critical
    max_depth_reached_ = std::max(max_depth_reached_, depth);

    double size = (max.x() - min.x());
    // G4cout << " Node size: " << size/um << " um" << G4endl;

    bool should_create_children = (size > 2.0 * minStepSize_) && (depth < max_depth_);

    // G4cout << " Size > 2*minStepSize: " << (size > 2.0 * minStepSize_) 
    //        << " (minStepSize: " << minStepSize_/um << " um)" << G4endl;
    // G4cout << " Depth < max_depth: " << (depth < max_depth_) 
    //        << " (depth: " << depth << ", max_depth: " << max_depth_ << ")" << G4endl;
    // G4cout << " Should create children: " << should_create_children << G4endl;

    if (should_create_children) {
        // G4cout << " Creating 8 children..." << G4endl;
        G4ThreeVector center = node->center;

        // Create children
        //#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < 8; ++i) {
            G4ThreeVector child_min, child_max;
            calculateChildBounds(min, max, center, i, child_min, child_max);
            // G4cout << " Child " << i << ": [" << child_min.x()/um << ", " 
            //        << child_min.y()/um << ", " << child_min.z()/um << "] um to [" 
            //        << child_max.x()/um << ", " << child_max.y()/um << ", " 
            //        << child_max.z()/um << "] um" << G4endl;

            //#pragma omp critical
            node->children[i] = createOctreeFromScratch(child_min, child_max, depth + 1);
        }

        node->is_leaf = false;
        // G4cout << " Created internal node with 8 children" << G4endl;
    } else {
        // This is a leaf node - compute field directly from charges
        // G4cout << " Creating LEAF node at center: [" << node->center.x()/um << ", " 
        //        << node->center.y()/um << ", " << node->center.z()/um << "] um" << G4endl;

        node->is_leaf = true;

        // Check if we have charges available
        if (fPositions.empty()) {
            G4cout << " WARNING: No charges available - field will be zero!" << G4endl;
            node->precomputed_field = G4ThreeVector(0,0,0);
        } else {
            node->precomputed_field = computeFieldFromCharges(node->center);
        }

        #pragma omp atomic
        leaf_nodes_++;

        // G4cout << " Leaf node created. Total leaves: " << leaf_nodes_ << G4endl;
        G4cout << " Field at leaf: [" << node->precomputed_field.x()/(volt/meter) << ", "
               << node->precomputed_field.y()/(volt/meter) << ", "
               << node->precomputed_field.z()/(volt/meter) << "] V/m" << G4endl;
    }

    // G4cout << "=== Returning from createOctreeFromScratch #" << node_count 
    //        << " (depth " << depth << ") ===" << G4endl;

    return node;
}


void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node, int depth) {
    if (!node) return;
    
    static int call_count = 0;
    call_count++;
    
    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x());

        // Check if we should refine based on gradient and size constraints
        if (size > minStepSize_ && depth < max_depth_) {
            G4cout << "  Checking gradient at center [" << node->center.x()/um << ", " 
                   << node->center.y()/um << ", " << node->center.z()/um << "] um" << G4endl;
            
            bool high_gradient = hasHighFieldGradient(node->center, node->precomputed_field, size * 0.2);
            G4cout << "  High gradient detected: " << high_gradient << G4endl;
            
            if (high_gradient) {
                #pragma omp atomic
                gradient_refinements_++;
                
                G4cout << "*** REFINING NODE! gradient_refinements_ = " << gradient_refinements_ << G4endl;
                
                // Convert this leaf to an internal node and create children
                node->is_leaf = false;
                #pragma omp atomic
                leaf_nodes_--;
                
                G4ThreeVector center = node->center;
                
                #pragma omp parallel for schedule(dynamic)
                for (int i = 0; i < 8; ++i) {
                    G4ThreeVector child_min, child_max;
                    calculateChildBounds(node->min, node->max, center, i, child_min, child_max);

                    auto child = std::make_unique<Node>();
                    child->min = child_min;
                    child->max = child_max;
                    child->center = (child_min + child_max) * 0.5;
                    child->is_leaf = true;
                    child->was_gradient_refined_ = true;  // Mark as gradient-refined

                    if (fPositions.empty()) {
                        child->precomputed_field = G4ThreeVector(0,0,0);
                    } else {
                        child->precomputed_field = computeFieldFromCharges(child->center);
                    }

                    #pragma omp atomic
                    total_nodes_++;
                    #pragma omp atomic
                    leaf_nodes_++;

                    #pragma omp critical
                    node->children[i] = std::move(child);
                }

                // Recursively check new children for refinement
                // for (auto& child : node->children) {
                //     //#pragma omp task firstprivate(child)
                //     #pragma omp task firstprivate(raw_child_ptr)
                //     refineMeshByGradient(child.get(), depth + 1);
                // }
                // #pragma omp taskwait

                for (auto& child : node->children) {
                    Node* raw_ptr = child.get(); // get raw pointer, no ownership transfer
                    #pragma omp task firstprivate(raw_ptr)
                    {
                        refineMeshByGradient(raw_ptr, depth + 1);
                    }
                }
                #pragma omp taskwait


            }
        } 
    } else {
        G4cout << "  Internal node - recursing to children" << G4endl;
        // Recursively check all children
        for (auto& child : node->children) {
            refineMeshByGradient(child.get(), depth + 1);
        }
    }
}

bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field, double sample_distance) const {
    G4cout << "    hasHighFieldGradient - center: [" << center.x()/um << ", " << center.y()/um << ", " 
           << center.z()/um << "] um, sample_distance: " << sample_distance/um << " um" << G4endl;
    
    // Check if charges are available
    if (fPositions.empty()) {
        G4cout << "    -> REJECTED: No charges available for gradient calculation" << G4endl;
        return false;
    }
    
    double actual_distance = std::min(sample_distance, 10.0 * um);
    
    std::vector<G4ThreeVector> sample_points = {
        center + G4ThreeVector(actual_distance, 0, 0),
        center - G4ThreeVector(actual_distance, 0, 0),
        center + G4ThreeVector(0, actual_distance, 0), 
        center - G4ThreeVector(0, actual_distance, 0),
        center + G4ThreeVector(0, 0, actual_distance),
        center - G4ThreeVector(0, 0, actual_distance)
    };
    
    std::vector<double> field_magnitudes;
    field_magnitudes.reserve(6);
    
    for (const auto& point : sample_points) {
        G4ThreeVector field = computeFieldFromCharges(point);
        double mag = field.mag();
        G4cout << "      Point [" << point.x()/um << ", " << point.y()/um << ", " << point.z()/um 
               << "] um -> field mag: " << mag/(volt/meter) << " V/m" << G4endl;
        field_magnitudes.push_back(mag);
    }
    
    double grad_x = (field_magnitudes[0] - field_magnitudes[1]) / (2 * actual_distance);
    double grad_y = (field_magnitudes[2] - field_magnitudes[3]) / (2 * actual_distance);
    double grad_z = (field_magnitudes[4] - field_magnitudes[5]) / (2 * actual_distance);
    
    double gradient_magnitude = std::sqrt(grad_x * grad_x + grad_y * grad_y + grad_z * grad_z);
    
    G4cout << "    -> Gradient magnitude: " << gradient_magnitude 
           << " V/m, normalized: " << gradient_magnitude / 1e6 
           << " vs threshold: " << fieldGradThreshold_ << G4endl;
    
    bool result = gradient_magnitude / 1e6 > fieldGradThreshold_;
    G4cout << "    -> Result: " << result << G4endl;
    
    return result;
}

// Remove getFieldFromUniformMap since we don't use uniform mesh anymore

G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const {
    // Add charge verification
    if (fPositions.empty()) {
        G4cout << "WARNING: computeFieldFromCharges called with empty fPositions!" << G4endl;
        return G4ThreeVector(0,0,0);
    }
    
    const G4double coulombsConstant = 1.0 / (4.0 * CLHEP::pi * epsilon0);
    const G4double epsilon = 1e-16;
    
    G4double Ex = 0.0, Ey = 0.0, Ez = 0.0;
    
    #pragma omp parallel for reduction(+:Ex,Ey,Ez) schedule(static)
    for (size_t i = 0; i < fPositions.size(); ++i) {
        const G4double dx = point.x() - fPositions[i].x();
        const G4double dy = point.y() - fPositions[i].y();
        const G4double dz = point.z() - fPositions[i].z();
        
        const G4double d2 = dx*dx + dy*dy + dz*dz;
        
        if (d2 > epsilon) {
            const G4double invd = 1.0 / std::sqrt(d2);
            const G4double invr3 = invd * invd * invd;
            const G4double scaled = fCharges[i] * coulombsConstant * invr3;
            
            Ex += dx * scaled;
            Ey += dy * scaled;
            Ez += dz * scaled;
        }
    }
    
    return G4ThreeVector(Ex, Ey, Ez);
}

void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const {
    const G4ThreeVector r(point[0], point[1], point[2]);
    G4ThreeVector E = evaluateField(r, root_.get());
    
    field[0] = field[1] = field[2] = 0.0;
    field[3] = E.x();
    field[4] = E.y();
    field[5] = E.z();
}

G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point, const Node* node) const {
    if (!node) return G4ThreeVector(0,0,0);
    
    if (node->is_leaf) {
        return computeFieldFromCharges(point);
    }
    
    for (const auto& child : node->children) {
        if (child && pointInside(child->min, child->max, point)) {
            return evaluateField(point, child.get());
        }
    }
    
    return computeFieldFromCharges(point);
}

bool AdaptiveSumRadialFieldMap::pointInside(const G4ThreeVector& min, const G4ThreeVector& max,
                                           const G4ThreeVector& point) const {
    return (point.x() >= min.x() && point.x() <= max.x() &&
            point.y() >= min.y() && point.y() <= max.y() &&
            point.z() >= min.z() && point.z() <= max.z());
}

void AdaptiveSumRadialFieldMap::calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max) {
    min = worldMin_;
    max = worldMax_;
}

void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const {
    G4cout << "=== Adaptive Mesh Statistics ===" << G4endl;
    G4cout << "Total nodes: " << total_nodes_ << G4endl;
    G4cout << "Leaf nodes: " << leaf_nodes_ << G4endl;
    G4cout << "Max depth reached: " << max_depth_reached_ << G4endl;
    G4cout << "Gradient-based refinements: " << gradient_refinements_ << G4endl;
    G4cout << "=================================" << G4endl;
}

void AdaptiveSumRadialFieldMap::collectStatistics(const Node* node, int depth) const {
    if (!node) return;
    
    if (!node->is_leaf) {
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < 8; ++i) {
            collectStatistics(node->children[i].get(), depth + 1);
        }
    }
}


void AdaptiveSumRadialFieldMap::calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                                                    const G4ThreeVector& center, int child_index,
                                                    G4ThreeVector& child_min, G4ThreeVector& child_max) const {
    switch (child_index) {
        case 0:
            child_min = min;
            child_max = center;
            break;
        case 1:
            child_min = G4ThreeVector(center.x(), min.y(), min.z());
            child_max = G4ThreeVector(max.x(), center.y(), center.z());
            break;
        case 2:
            child_min = G4ThreeVector(min.x(), center.y(), min.z());
            child_max = G4ThreeVector(center.x(), max.y(), center.z());
            break;
        case 3:
            child_min = G4ThreeVector(center.x(), center.y(), min.z());
            child_max = G4ThreeVector(max.x(), max.y(), center.z());
            break;
        case 4:
            child_min = G4ThreeVector(min.x(), min.y(), center.z());
            child_max = G4ThreeVector(center.x(), center.y(), max.z());
            break;
        case 5:
            child_min = G4ThreeVector(center.x(), min.y(), center.z());
            child_max = G4ThreeVector(max.x(), center.y(), max.z());
            break;
        case 6:
            child_min = G4ThreeVector(min.x(), center.y(), center.z());
            child_max = G4ThreeVector(center.x(), max.y(), max.z());
            break;
        case 7:
            child_min = center;
            child_max = max;
            break;
    }
}
void AdaptiveSumRadialFieldMap::ExportFieldMapToFile(const std::string& filename) const {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        G4Exception("AdaptiveSumRadialFieldMap::ExportFieldMapToFile",
                    "FileOpenError", FatalException,
                    ("Failed to open file: " + filename).c_str());
        return;
    }
    
    // Write header with adaptive mesh metadata
    double world_bounds[6] = {
        worldMin_.x(), worldMin_.y(), worldMin_.z(),
        worldMax_.x(), worldMax_.y(), worldMax_.z()
    };
    
    // FIX: Use explicit static_cast to avoid narrowing warnings
    int mesh_parameters[5] = {
        max_depth_,
        static_cast<int>(minStepSize_ / um),
        static_cast<int>(total_nodes_),
        static_cast<int>(leaf_nodes_),
        static_cast<int>(fStorage)
    };
    
    // Write world bounds and mesh parameters
    outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));
    
    // Write only the field data from leaf nodes (calculated field points)
    writeFieldPointsToFile(outfile, root_.get());
    
    // FIX: Use explicit static_cast for statistics to avoid narrowing warnings
    int statistics[3] = {
        static_cast<int>(gradient_refinements_),  // FIXED: Add static_cast
        max_depth_reached_,
        static_cast<int>(fPositions.size())
    };
    outfile.write(reinterpret_cast<const char*>(statistics), sizeof(statistics));
    
    outfile.close();
    G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
    G4cout << "  Field points: " << leaf_nodes_ << " points, Total nodes: " << total_nodes_ 
           << ", Gradient refinements: " << gradient_refinements_ << G4endl;
}

void AdaptiveSumRadialFieldMap::writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const {
    if (!node) return;

    if (node->is_leaf) {
        double position[3] = {
            node->center.x(),
            node->center.y(),
            node->center.z()
        };

        double field_value[3] = {
            node->precomputed_field.x() / (volt/meter),
            node->precomputed_field.y() / (volt/meter),
            node->precomputed_field.z() / (volt/meter)
        };

        outfile.write(reinterpret_cast<const char*>(position), sizeof(position));
        outfile.write(reinterpret_cast<const char*>(field_value), sizeof(field_value));
    } else {
        for (const auto& child : node->children) {
            writeFieldPointsToFile(outfile, child.get());
        }
    }
}
