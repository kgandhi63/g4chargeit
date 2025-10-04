// AdaptiveSumRadialFieldMap.hh
#ifndef AdaptiveSumRadialFieldMap_h
#define AdaptiveSumRadialFieldMap_h

#include "G4ThreeVector.hh"
#include "G4VSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4ElectricField.hh"
#include "SumRadialFieldMap.hh"  // Include the uniform field map
#include <array>
#include <memory>
#include <vector>
#include <string>
#include <fstream>

class AdaptiveSumRadialFieldMap : public G4ElectricField {
public:
    enum class StorageType { Float, Double };

    struct Node {
        G4ThreeVector min, max;
        G4ThreeVector center;
        G4ThreeVector precomputed_field;  // Field value at center
        bool is_leaf;
        std::array<std::unique_ptr<Node>, 8> children;
        
        Node() : is_leaf(false) {}
    };

    // Constructor that takes a uniform mesh from SumRadialFieldMap
    AdaptiveSumRadialFieldMap(std::unique_ptr<G4VSolid> solid, 
                             const SumRadialFieldMap& uniformFieldMap,
                             const G4double& gradThreshold,
                             const G4double& minStep,
                             const std::string& filename,
                             const G4ThreeVector& min,
                             const G4ThreeVector& max,
                             int max_depth,
                             StorageType storage = StorageType::Double);

    ~AdaptiveSumRadialFieldMap() override = default;

    // Get electric field at point
    void GetFieldValue(const G4double point[4], G4double field[6]) const override;

    // Export field map
    void ExportFieldMapToFile(const std::string& filename) const;

    // Mesh statistics
    void PrintMeshStatistics() const;

private:
    std::unique_ptr<Node> root_;
    std::unique_ptr<G4VSolid> sphereSolid_;
    int max_depth_;
    
    // Field data from uniform mesh
    std::vector<G4ThreeVector> fPositions;
    std::vector<G4double> fCharges;
    G4double worldX_, worldY_,worldZ_; 
    G4ThreeVector worldMin_, worldMax_; 
    G4double minStepSize_;
    StorageType fStorage;

    // Gradient-based refinement parameters
    double fieldGradThreshold_;  // V/m per micron

    // Mesh statistics
    mutable size_t total_nodes_;
    mutable size_t leaf_nodes_;
    mutable size_t gradient_refinements_;
    mutable int max_depth_reached_;

    // Mesh construction methods
    std::unique_ptr<Node> buildFromUniformMesh(const SumRadialFieldMap& uniformFieldMap);
    std::unique_ptr<Node> createOctreeFromUniformGrid(const SumRadialFieldMap& uniformFieldMap, 
                                                     const G4ThreeVector& min, 
                                                     const G4ThreeVector& max,
                                                     int depth);
    void refineMeshByGradient(Node* node);
    
    void calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max);
    
    // Gradient-based refinement methods
    bool hasHighFieldGradient(const G4ThreeVector& center, double sample_distance) const;
    double estimateFieldGradient(const G4ThreeVector& center, double distance) const;
    G4ThreeVector computeFieldFromCharges(const G4ThreeVector& point) const;
    G4ThreeVector getFieldFromUniformMap(const G4ThreeVector& point, const SumRadialFieldMap& uniformMap) const;

    // Field computation
    G4ThreeVector evaluateField(const G4ThreeVector& point, const Node* node) const;
    
    // Utility functions
    void calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                             const G4ThreeVector& center, int child_index,
                             G4ThreeVector& child_min, G4ThreeVector& child_max) const;
    
    bool pointInside(const G4ThreeVector& min, const G4ThreeVector& max, 
                    const G4ThreeVector& point) const;

    // Export helper functions
    void writeOctreeToFile(std::ofstream& outfile, const Node* node) const;

    // Statistics collection
    void collectStatistics(const Node* node, int depth) const;

    // Set world dimensions
    void setWorldDimensions(double x, double y, double z) {
        worldX_ = x;
        worldY_ = y;
        worldZ_ = z;
    }
};

#endif // AdaptiveSumRadialFieldMap_h