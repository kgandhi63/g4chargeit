#include "AdaptiveSumRadialFieldMap.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VSolid.hh"
#include "G4Exception.hh"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <stdexcept>
#include <immintrin.h>
#include <iomanip>

AdaptiveSumRadialFieldMap::AdaptiveSumRadialFieldMap(
    std::unique_ptr<G4VSolid> solid, 
    const SumRadialFieldMap& uniformFieldMap,
    const G4double& gradThreshold,
    const G4double& minStep,
    const std::string& filename,
    const G4ThreeVector& min,
    const G4ThreeVector& max,
    int max_depth,
    StorageType storage)
    : sphereSolid_(std::move(solid)), max_depth_(max_depth),minStepSize_(minStep),
      worldMin_(min), worldMax_(max), fieldGradThreshold_(gradThreshold), fStorage(storage)
{
                 
    // Set world dimensions
    setWorldDimensions(worldMax_.x() - worldMin_.x(), worldMax_.y() - worldMin_.y(), worldMax_.z() - worldMin_.z());

    // Initialize statistics
    total_nodes_ = 0;
    leaf_nodes_ = 0;
    gradient_refinements_ = 0;
    max_depth_reached_ = 0;

    // Build adaptive mesh from uniform mesh
    root_ = buildFromUniformMesh(uniformFieldMap);

    G4cout << "Adaptive mesh built from uniform mesh" << G4endl;

    // Collect final statistics
    collectStatistics(root_.get(), 0);

    // Export if filename provided
    if (!filename.empty()) {
        ExportFieldMapToFile(filename);
    }
    
    PrintMeshStatistics();
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildFromUniformMesh(const SumRadialFieldMap& uniformFieldMap) {
    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    
    // Create initial octree matching the uniform grid resolution
    G4cout << "Creating initial octree from uniform grid..." << G4endl;
    auto root = createOctreeFromUniformGrid(uniformFieldMap, min, max, 0);
    
    // Refine based on field gradients using the uniform map for field queries
    G4cout << "Refining mesh based on field gradients..." << G4endl;
    refineMeshByGradient(root.get());
    
    return root;
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::createOctreeFromUniformGrid(const SumRadialFieldMap& uniformFieldMap,
                                                      const G4ThreeVector& min, 
                                                      const G4ThreeVector& max,
                                                      int depth) {
    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;
    
    #pragma omp atomic
    total_nodes_++;

    #pragma omp critical
    max_depth_reached_ = std::max(max_depth_reached_, depth);

    double size = (max.x() - min.x());
    
    // Determine if we should create children based on uniform grid resolution
    // Stop when we reach approximately the uniform grid cell size
    bool should_create_children = (size > 2.0 * minStepSize_) && (depth < max_depth_);
    
    if (should_create_children) {
        G4ThreeVector center = node->center;
        
        // Create children
        for (int i = 0; i < 8; ++i) {
            G4ThreeVector child_min, child_max;
            calculateChildBounds(min, max, center, i, child_min, child_max);
            node->children[i] = createOctreeFromUniformGrid(uniformFieldMap, child_min, child_max, depth + 1);
        }
        node->is_leaf = false;
    } else {
        // This is a leaf node - use uniform field map to get field value
        node->is_leaf = true;
        node->precomputed_field = getFieldFromUniformMap(node->center, uniformFieldMap);
        
        #pragma omp atomic
        leaf_nodes_++;
    }
    
    return node;
}

void AdaptiveSumRadialFieldMap::refineMeshByGradient(Node* node) {
    if (!node) return;
    
    // If this is a leaf node, check if it needs refinement
    if (node->is_leaf) {
        double size = (node->max.x() - node->min.x());
        
        // Check if we should refine based on gradient and size constraints
        if (size > minStepSize_ && hasHighFieldGradient(node->center, size * 0.2)) {
            #pragma omp atomic
            gradient_refinements_++;
            
            G4cout << "Refining node at [" << node->center.x()/um << ", " 
                   << node->center.y()/um << ", " << node->center.z()/um 
                   << "] um, size: " << size/um << " um" << G4endl;
            
            // Convert this leaf to an internal node and create children
            node->is_leaf = false;
            #pragma omp atomic
            leaf_nodes_--;
            
            G4ThreeVector center = node->center;
            
            // Create children as leaf nodes initially
            for (int i = 0; i < 8; ++i) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(node->min, node->max, center, i, child_min, child_max);
                
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
                
                node->children[i] = std::move(child);
            }
            
            // Recursively check new children for refinement
            for (auto& child : node->children) {
                refineMeshByGradient(child.get());
            }
        }
    } else {
        // Recursively check all children
        for (auto& child : node->children) {
            refineMeshByGradient(child.get());
        }
    }
}

G4ThreeVector AdaptiveSumRadialFieldMap::getFieldFromUniformMap(const G4ThreeVector& point, 
                                                               const SumRadialFieldMap& uniformMap) const {
    // Use the uniform field map to get the field value
    G4double field[6] = {0};
    G4double point4[4] = {point.x(), point.y(), point.z(), 0.0};
    uniformMap.GetFieldValue(point4, field);
    
    return G4ThreeVector(field[3], field[4], field[5]);  // Electric field components
}

bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(const G4ThreeVector& center, double sample_distance) const {
    // Quick check for very large nodes
    if (sample_distance > 50 * um) {
        return false;
    }
    
    // Estimate field gradient
    double gradient_magnitude = estimateFieldGradient(center, sample_distance);
    
    // Convert to V/m per micron for threshold comparison
    double gradient_vm_per_um = gradient_magnitude / (1e6 * volt/meter * um);
    
    return gradient_vm_per_um > fieldGradThreshold_;
}

double AdaptiveSumRadialFieldMap::estimateFieldGradient(const G4ThreeVector& center, double distance) const {
    double actual_distance = std::min(distance, 5.0 * um);
    
    // Sample field at 6 points around center
    std::vector<G4ThreeVector> sample_points = {
        center + G4ThreeVector(actual_distance, 0, 0),
        center - G4ThreeVector(actual_distance, 0, 0),
        center + G4ThreeVector(0, actual_distance, 0),
        center - G4ThreeVector(0, actual_distance, 0),
        center + G4ThreeVector(0, 0, actual_distance),
        center - G4ThreeVector(0, 0, actual_distance)
    };
    
    // Compute field magnitudes at sample points
    std::vector<double> field_magnitudes;
    field_magnitudes.reserve(6);
    
    for (const auto& point : sample_points) {
        G4ThreeVector field = computeFieldFromCharges(point);
        field_magnitudes.push_back(field.mag());
    }
    
    // Calculate gradient components
    double grad_x = (field_magnitudes[0] - field_magnitudes[1]) / (2 * actual_distance);
    double grad_y = (field_magnitudes[2] - field_magnitudes[3]) / (2 * actual_distance);
    double grad_z = (field_magnitudes[4] - field_magnitudes[5]) / (2 * actual_distance);
    
    return std::sqrt(grad_x * grad_x + grad_y * grad_y + grad_z * grad_z);
}

G4ThreeVector AdaptiveSumRadialFieldMap::computeFieldFromCharges(const G4ThreeVector& point) const {
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

// Implementation of required GetFieldValue method
void AdaptiveSumRadialFieldMap::GetFieldValue(const G4double point[4], G4double field[6]) const {
    const G4ThreeVector r(point[0], point[1], point[2]);
    G4ThreeVector E = evaluateField(r, root_.get());
    
    // Return as electric field; no magnetic component
    field[0] = field[1] = field[2] = 0.0;  // B-field components
    field[3] = E.x();  // E-field x
    field[4] = E.y();  // E-field y  
    field[5] = E.z();  // E-field z
}

G4ThreeVector AdaptiveSumRadialFieldMap::evaluateField(const G4ThreeVector& point, const Node* node) const {
    if (!node) return G4ThreeVector(0,0,0);
    
    if (node->is_leaf) {
        // For leaf nodes close to center, we could use precomputed values
        // But for accuracy, always compute directly from charges
        return computeFieldFromCharges(point);
    }
    
    // Find which child contains the point
    for (const auto& child : node->children) {
        if (child && pointInside(child->min, child->max, point)) {
            return evaluateField(point, child.get());
        }
    }
    
    // Fallback: compute field directly
    return computeFieldFromCharges(point);
}

G4ThreeVector AdaptiveSumRadialFieldMap::computeLeafField(const G4ThreeVector& point, const Node* node) const {
    // Delegate to the main field computation function
    return computeFieldFromCharges(point);
}

void AdaptiveSumRadialFieldMap::PrintMeshStatistics() const {
    G4cout << "=== Adaptive Mesh Statistics ===" << G4endl;
    G4cout << "Total nodes: " << total_nodes_ << G4endl;
    G4cout << "Leaf nodes: " << leaf_nodes_ << G4endl;
    G4cout << "Max depth reached: " << max_depth_reached_ << G4endl;
    G4cout << "Gradient-based refinements: " << gradient_refinements_ << G4endl;
    G4cout << "Field gradient threshold: " << fieldGradThreshold_ << " V/m per µm" << G4endl;
    G4cout << "Min node size: " << minStepSize_/um << " µm" << G4endl;
    G4cout << "=================================" << G4endl;
}

void AdaptiveSumRadialFieldMap::collectStatistics(const Node* node, int depth) const {
    if (!node) return;
    
    if (node->is_leaf) {
        // Can collect additional leaf statistics here if needed
    } else {
        for (const auto& child : node->children) {
            collectStatistics(child.get(), depth + 1);
        }
    }
}

void AdaptiveSumRadialFieldMap::calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                                                    const G4ThreeVector& center, int child_index,
                                                    G4ThreeVector& child_min, G4ThreeVector& child_max) const {
    // Calculate bounds for each of the 8 octree children
    switch (child_index) {
        case 0: // -X, -Y, -Z
            child_min = min;
            child_max = center;
            break;
        case 1: // +X, -Y, -Z
            child_min = G4ThreeVector(center.x(), min.y(), min.z());
            child_max = G4ThreeVector(max.x(), center.y(), center.z());
            break;
        case 2: // -X, +Y, -Z
            child_min = G4ThreeVector(min.x(), center.y(), min.z());
            child_max = G4ThreeVector(center.x(), max.y(), center.z());
            break;
        case 3: // +X, +Y, -Z
            child_min = G4ThreeVector(center.x(), center.y(), min.z());
            child_max = G4ThreeVector(max.x(), max.y(), center.z());
            break;
        case 4: // -X, -Y, +Z
            child_min = G4ThreeVector(min.x(), min.y(), center.z());
            child_max = G4ThreeVector(center.x(), center.y(), max.z());
            break;
        case 5: // +X, -Y, +Z
            child_min = G4ThreeVector(center.x(), min.y(), center.z());
            child_max = G4ThreeVector(max.x(), center.y(), max.z());
            break;
        case 6: // -X, +Y, +Z
            child_min = G4ThreeVector(min.x(), center.y(), center.z());
            child_max = G4ThreeVector(center.x(), max.y(), max.z());
            break;
        case 7: // +X, +Y, +Z
            child_min = center;
            child_max = max;
            break;
    }
}

bool AdaptiveSumRadialFieldMap::pointInside(const G4ThreeVector& min, const G4ThreeVector& max, 
                                           const G4ThreeVector& point) const {
    return (point.x() >= min.x() && point.x() <= max.x() &&
            point.y() >= min.y() && point.y() <= max.y() &&
            point.z() >= min.z() && point.z() <= max.z());
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
    int mesh_parameters[4] = {
        max_depth_,
        static_cast<int>(minStepSize_ / um), // Convert to microns for storage
        static_cast<int>(fPositions.size()), // Number of charges
        static_cast<int>(fStorage)           // Storage type
    };
    
    // Write world bounds and mesh parameters
    outfile.write(reinterpret_cast<const char*>(world_bounds), sizeof(world_bounds));
    outfile.write(reinterpret_cast<const char*>(mesh_parameters), sizeof(mesh_parameters));
    
    // Write charge positions and values
    for (size_t i = 0; i < fPositions.size(); ++i) {
        double charge_data[4] = {
            fPositions[i].x(), fPositions[i].y(), fPositions[i].z(), fCharges[i]
        };
        outfile.write(reinterpret_cast<const char*>(charge_data), sizeof(charge_data));
    }
    
    // Write octree structure recursively
    writeOctreeToFile(outfile, root_.get());
    
    outfile.close();
    G4cout << "Adaptive binary field map exported to: " << filename << G4endl;
}

// Helper function to recursively write octree structure
void AdaptiveSumRadialFieldMap::writeOctreeToFile(std::ofstream& outfile, const Node* node) const {
    if (!node) return;
    
    // Write node header: bounds and node type
    double node_bounds[6] = {
        node->min.x(), node->min.y(), node->min.z(),
        node->max.x(), node->max.y(), node->max.z()
    };
    char node_type = node->is_leaf ? 1 : 0;
    
    outfile.write(reinterpret_cast<const char*>(node_bounds), sizeof(node_bounds));
    outfile.write(&node_type, sizeof(node_type));
    
    // if (node->is_leaf) {
    //     // For leaf nodes, write charge density
    //     outfile.write(reinterpret_cast<const char*>(&node->charge), sizeof(node->charge));
    // } else {
    //     // For non-leaf nodes, recursively write children
    //     for (const auto& child : node->children) {
    //         writeOctreeToFile(outfile, child.get());
    //     }
    //}
}