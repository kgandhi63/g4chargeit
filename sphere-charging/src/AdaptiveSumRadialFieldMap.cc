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
    const std::vector<G4ThreeVector>& positions,
    const std::vector<double>& charges,
    const G4ThreeVector& min,
    const G4ThreeVector& max,
    const G4double& minStepSize,
    const G4double& gradThreshold,
    const std::string& filename,
    int max_depth,
    StorageType storage)
    : sphereSolid_(std::move(solid)), max_depth_(max_depth),
      fPositions(positions), fCharges(charges), fieldGradThreshold_(gradThreshold),
      worldMin_(min), worldMax_(max), minStepSize_(minStepSize), fStorage(storage)
{
    // Basic validation

    if (positions.size() != charges.size()) {
        throw std::runtime_error("AdaptiveSumRadialFieldMap: positions and charges size mismatch.");
    }
    
    if (worldMax_.x() <= worldMin_.x() || worldMax_.y() <= worldMin_.y() || worldMax_.z() <= worldMin_.z()) {
        throw std::runtime_error("AdaptiveSumRadialFieldMap: max must exceed min in all axes.");
    }

    // Initialize statistics
    total_nodes_ = 0;
    leaf_nodes_ = 0;
    surface_refinements_ = 0;
    gradient_refinements_ = 0;
    max_depth_reached_ = 0;

    // Set world dimensions from input bounds
    setWorldDimensions(worldMax_.x() - worldMin_.x(), worldMax_.y() - worldMin_.y(), worldMax_.z() - worldMin_.z());
    
    // Build the adaptive mesh with OpenMP parallelization
    root_ = buildAdaptiveMesh();

    G4cout << "after build mesh" << G4endl;

    // Collect final statistics
    collectStatistics(root_.get(), 0);

    // Export if filename provided
    if (!filename.empty()) {
        ExportFieldMapToFile(filename);
    }
    
    PrintMeshStatistics();
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildAdaptiveMesh() {
    G4ThreeVector min, max;
    calculateBoundingBox(min, max);
    
    return buildOctree(min, max, 0);
}

void AdaptiveSumRadialFieldMap::calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max) {
    min = worldMin_;
    max = worldMax_;
}

std::unique_ptr<AdaptiveSumRadialFieldMap::Node> 
AdaptiveSumRadialFieldMap::buildOctree(const G4ThreeVector& min, 
                                      const G4ThreeVector& max,
                                      int depth) {
    auto node = std::make_unique<Node>();
    node->min = min;
    node->max = max;
    node->center = (min + max) * 0.5;
    node->is_leaf = false;

    #pragma omp atomic
    total_nodes_++;

    #pragma omp critical
    max_depth_reached_ = std::max(max_depth_reached_, depth);

    double size = (max.x() - min.x());
    G4cout << "Depth: " << depth << ", Size: " << size/um << " um, "
           << "Bounds: [" << min.x()/um << "," << min.y()/um << "," << min.z()/um << "] to ["
           << max.x()/um << "," << max.y()/um << "," << max.z()/um << "]" << G4endl;

    // Check if we should refine this node based on multiple criteria
    bool should_refine = shouldRefineNode(min, max, node->center, depth);
    G4cout << "Should refine: " << should_refine << G4endl;
    
    if (should_refine && depth < max_depth_) {
        G4cout << "REFINING - Creating children at depth " << depth << G4endl;
        G4ThreeVector center = node->center;
        
        // Add a safety check to prevent infinite recursion
        if (size < 0.1 * um) {
            G4cout << "WARNING: Node size too small, forcing leaf node" << G4endl;
            node->is_leaf = true;
            #pragma omp atomic
            leaf_nodes_++;
            return node;
        }
        
        // Parallelize child creation for deeper levels
        if (depth >= 2) {
            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < 8; ++i) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(min, max, center, i, child_min, child_max);
                G4cout << "Child " << i << " bounds calculated" << G4endl;
                
                #pragma omp critical
                {
                    node->children[i] = buildOctree(child_min, child_max, depth + 1);
                }
                G4cout << "Child " << i << " completed" << G4endl;
            }
        } else {
            // Sequential for top levels (better cache locality)
            for (int i = 0; i < 8; ++i) {
                G4ThreeVector child_min, child_max;
                calculateChildBounds(min, max, center, i, child_min, child_max);
                G4cout << "Child " << i << " bounds calculated" << G4endl;
                node->children[i] = buildOctree(child_min, child_max, depth + 1);
                G4cout << "Child " << i << " completed" << G4endl;
            }
        }
        G4cout << "All children created at depth " << depth << G4endl;
    } else {
        // Leaf node
        G4cout << "MAKING LEAF NODE at depth " << depth << G4endl;
        node->is_leaf = true;
        #pragma omp atomic
        leaf_nodes_++;
    }
    
    G4cout << "Returning node from depth " << depth << G4endl;
    return node;
}

bool AdaptiveSumRadialFieldMap::shouldRefineNode(const G4ThreeVector& min, 
                                                const G4ThreeVector& max,
                                                const G4ThreeVector& center,
                                                int depth) {
    double size = (max.x() - min.x());
    
    // Always refine first few levels to establish base structure
    if (depth < 2) return true;
    
    // Check minimum size constraint
    if (size < minStepSize_) {
        return false;
    }


    
    // Check if node is near CAD surface - refine these regions
    // if (isNearSurface(center, size * 0.3)) {
    //     #pragma omp atomic
    //     surface_refinements_++;
    //     return true;
    // }
    
    // Check field gradient - refine regions with high field variation
    if (hasHighFieldGradient(center, size * 0.2)) {
        #pragma omp atomic
        gradient_refinements_++;
        return true;
    }
    
    return false;
}

bool AdaptiveSumRadialFieldMap::isNearSurface(const G4ThreeVector& point, double tolerance) const {
    if (!sphereSolid_) return false;
    
    EInside inside_status = sphereSolid_->Inside(point);
    
    // Refine if inside or on surface
    if (inside_status == kInside || inside_status == kSurface) {
        return true;
    }
    
    return false;
}

bool AdaptiveSumRadialFieldMap::hasHighFieldGradient(const G4ThreeVector& center, double sample_distance) const {
    // Estimate field gradient by sampling around the center point
    double gradient_magnitude = estimateFieldGradient(center, sample_distance);
    
    // Convert to V/m per micron for threshold comparison
    double gradient_vm_per_um = gradient_magnitude / (1e6 * volt/meter * um);
    
    return gradient_vm_per_um > fieldGradThreshold_;
}

double AdaptiveSumRadialFieldMap::estimateFieldGradient(const G4ThreeVector& center, double distance) const {
    // Sample field at 6 points around center to estimate gradient components
    std::vector<G4ThreeVector> sample_points = {
        center + G4ThreeVector(distance, 0, 0),
        center - G4ThreeVector(distance, 0, 0),
        center + G4ThreeVector(0, distance, 0),
        center - G4ThreeVector(0, distance, 0),
        center + G4ThreeVector(0, 0, distance),
        center - G4ThreeVector(0, 0, distance)
    };
    
    // Compute fields at sample points
    std::vector<G4ThreeVector> sample_fields;
    sample_fields.reserve(6);
    
    for (const auto& point : sample_points) {
        sample_fields.push_back(computeFieldFromCharges(point));
    }
    
    // Calculate gradient components using finite differences
    G4ThreeVector gradient;
    gradient.setX((sample_fields[0].mag() - sample_fields[1].mag()) / (2 * distance)); // dE/dx
    gradient.setY((sample_fields[2].mag() - sample_fields[3].mag()) / (2 * distance)); // dE/dy
    gradient.setZ((sample_fields[4].mag() - sample_fields[5].mag()) / (2 * distance)); // dE/dz
    
    return gradient.mag();
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
    G4cout << "Surface-based refinements: " << surface_refinements_ << G4endl;
    G4cout << "Gradient-based refinements: " << gradient_refinements_ << G4endl;
    G4cout << "Field gradient threshold: " << fieldGradThreshold_ << " V/m per µm" << G4endl;
    G4cout << "World size: " << worldX_/um << " µm cube" << G4endl;
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
    
    if (node->is_leaf) {
        // For leaf nodes, write charge density
        outfile.write(reinterpret_cast<const char*>(&node->charge), sizeof(node->charge));
    } else {
        // For non-leaf nodes, recursively write children
        for (const auto& child : node->children) {
            writeOctreeToFile(outfile, child.get());
        }
    }
}