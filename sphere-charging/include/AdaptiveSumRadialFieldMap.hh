#ifndef AdaptiveSumRadialFieldMap_h
#define AdaptiveSumRadialFieldMap_h

#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4ElectricField.hh"
#include <array>
#include <memory>
#include <vector>
#include <string>
#include <fstream>

class AdaptiveSumRadialFieldMap : public G4ElectricField {
public:
    enum class StorageType { Float, Double };

    // This Node is for the FIELD MAP (your existing octree)
    struct Node {
        G4ThreeVector min, max, center, precomputed_field;
        bool is_leaf;
        std::array<std::unique_ptr<Node>, 8> children;
        Node() : is_leaf(false) {}
    };

    AdaptiveSumRadialFieldMap(const std::vector<G4ThreeVector>& positions,
                              const std::vector<G4double>& charges,
                              const G4double& gradThreshold,
                              const G4double& minStep,
                              const std::string& filename,
                              const G4ThreeVector& min,
                              const G4ThreeVector& max,
                              int max_depth,
                              StorageType storage = StorageType::Double);

    ~AdaptiveSumRadialFieldMap() override = default;
    void GetFieldValue(const G4double point[4], G4double field[6]) const override;
    void ExportFieldMapToFile(const std::string& filename) const;
    void PrintMeshStatistics() const;

private:
    // --- NEW: A separate Node struct for the CHARGE OCTREE (for Barnes-Hut) ---
    struct ChargeNode {
        std::array<std::unique_ptr<ChargeNode>, 8> children;
        G4ThreeVector center_of_mass;
        G4double total_charge = 0.0;
        int particle_index = -1; // -1 if not a leaf with a single particle
        bool is_leaf = true;
    };

    // Main octree for the field map
    std::unique_ptr<Node> root_;
    int max_depth_;
    
    // Original charge data
    std::vector<G4ThreeVector> fPositions;
    std::vector<G4double> fCharges;
    G4double worldX_, worldY_, worldZ_;
    G4ThreeVector worldMin_, worldMax_;
    G4double minStepSize_;
    StorageType fStorage;

    // --- NEW: Members for the Barnes-Hut Charge Octree ---
    std::unique_ptr<ChargeNode> charge_root_;
    const double barnes_hut_theta_ = 0.5;

    // --- NEW: A vector to hold pointers to all leaf nodes for parallel computation ---
    std::vector<Node*> all_leaves_;

    double fieldGradThreshold_;
    mutable size_t total_nodes_, leaf_nodes_, gradient_refinements_;
    mutable int max_depth_reached_;

    // Field Map construction methods
    std::unique_ptr<Node> buildFromScratch();
    std::unique_ptr<Node> createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth);
    void refineMeshByGradient(Node* node, int depth);
    
    // --- NEW: Methods for building and using the Charge Octree ---
    void buildChargeOctree();
    void insertCharge(ChargeNode* node, int particle_index, const G4ThreeVector& min, const G4ThreeVector& max);
    
    // FIXED: Declaration now matches the implementation (4 arguments)
    G4ThreeVector computeFieldWithApproximation(const G4ThreeVector& point, const ChargeNode* node, const G4ThreeVector& node_min, const G4ThreeVector& node_max) const;

    G4ThreeVector computeFieldFromCharges(const G4ThreeVector& point) const;

    bool hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field, double sample_distance) const;
    G4ThreeVector evaluateField(const G4ThreeVector& point, const Node* node) const;
    
    // Utility functions
    // FIXED: Added const to allow calling from const functions
    void calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max) const;
    
    void calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max,
                              const G4ThreeVector& center, int child_index,
                              G4ThreeVector& child_min, G4ThreeVector& child_max) const;
    bool pointInside(const G4ThreeVector& min, const G4ThreeVector& max, const G4ThreeVector& point) const;
    void writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const;
    void collectStatistics(const Node* node, int depth) const;
    void setWorldDimensions(double x, double y, double z) { worldX_ = x; worldY_ = y; worldZ_ = z; }
};

#endif // AdaptiveSumRadialFieldMap_h
