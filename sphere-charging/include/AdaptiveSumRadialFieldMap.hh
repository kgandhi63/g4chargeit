#ifndef ADAPTIVESUMRADIALFIELDMAP_HH
#define ADAPTIVESUMRADIALFIELDMAP_HH

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include <string>
#include <memory>
#include <fstream> // Required for ofstream

class AdaptiveSumRadialFieldMap : public G4ElectricField {
public:
    // Forward-declare nested structs
    struct Node;
    struct ChargeNode;

    enum StorageType { Full, Sparse };

    // --- MODIFIED CONSTRUCTOR ---
    // Now accepts time step and temperature to perform the one-time "tax" calculation.
    AdaptiveSumRadialFieldMap(
        std::vector<G4ThreeVector>& positions, // Now a non-const reference
        std::vector<G4double>& charges,       // Now a non-const reference
        const G4double& gradThreshold,
        const G4double& minStep,
        const G4double& time_step_dt,      // Time step for dissipation
        const G4double& material_temp_K, // Material temperature in Kelvin
        const std::string& filename,
        const G4ThreeVector& min,
        const G4ThreeVector& max,
        int max_depth,
        StorageType storage);

    ~AdaptiveSumRadialFieldMap() override = default;

    void GetFieldValue(const G4double point[4], G4double field[6]) const override;

    // --- ADDED: PUBLIC GETTER ---
    const std::vector<Node*>& getLeafNodes() const { return all_leaves_; }
    void PrintMeshStatistics() const;

    // --- PUBLIC STRUCT DEFINITIONS ---
    struct Node {
        G4ThreeVector min, max, center;
        bool is_leaf = true;
        std::unique_ptr<Node> children[8];
        G4ThreeVector precomputed_field;
    };

    struct ChargeNode {
        G4ThreeVector center_of_mass;
        G4double total_charge = 0.0;
        bool is_leaf = true;
        int particle_index = -1; // -1: empty, -2: multi-particle, >=0: single particle
        std::unique_ptr<ChargeNode> children[8];
    };

private:
    // --- PRIVATE HELPER FUNCTIONS ---
    void buildChargeOctree();
    void insertCharge(ChargeNode* node, int particle_index, const G4ThreeVector& min, const G4ThreeVector& max);
    G4ThreeVector computeFieldFromCharges(const G4ThreeVector& point) const;
    G4ThreeVector computeFieldWithApproximation(const G4ThreeVector& point, const ChargeNode* node, const G4ThreeVector& node_min, const G4ThreeVector& node_max) const;

    std::unique_ptr<Node> buildFromScratch();
    std::unique_ptr<Node> createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth);
    void refineMeshByGradient(Node* node, int depth);
    bool hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field, double sample_distance) const;
    G4ThreeVector evaluateField(const G4ThreeVector& point, const Node* node) const;

    // --- ADDED: NEW PHYSICS FUNCTIONS ---
    void ApplyChargeDissipation(G4double dt, G4double temp_K);
    double calculateConductivity(G4double temp_K) const;
    void removeChargeFromRegion(const G4ThreeVector& min_bounds, const G4ThreeVector& max_bounds, G4double charge_to_remove);
    bool isNodeOnSurface(const Node* node) const;

    // Utility functions
    void calculateBoundingBox(G4ThreeVector& min, G4ThreeVector& max) const;
    void calculateChildBounds(const G4ThreeVector& min, const G4ThreeVector& max, const G4ThreeVector& center, int child_index, G4ThreeVector& child_min, G4ThreeVector& child_max) const;
    bool pointInside(const G4ThreeVector& min, const G4ThreeVector& max, const G4ThreeVector& point) const;
    void setWorldDimensions(G4double wx, G4double wy, G4double wz) {}

    // I/O and Statistics
    void ExportFieldMapToFile(const std::string& filename) const;
    void writeFieldPointsToFile(std::ofstream& outfile, const Node* node) const;
    void collectStatistics(const Node* node, int depth);

    // --- MEMBER VARIABLES ---
    std::unique_ptr<Node> root_;
    std::unique_ptr<ChargeNode> charge_root_;
    std::vector<Node*> all_leaves_;

    // Simulation parameters
    int max_depth_;
    G4double minStepSize_;
    G4ThreeVector worldMin_, worldMax_;
    G4double fieldGradThreshold_;
    StorageType fStorage;
    G4double barnes_hut_theta_ = 0.5;

    // --- MODIFIED: Particle data are now non-const references ---
    std::vector<G4ThreeVector>& fPositions;
    std::vector<G4double>& fCharges;

    // Statistics
    mutable int total_nodes_;
    mutable int leaf_nodes_;
    mutable int gradient_refinements_;
    mutable int max_depth_reached_;
};

#endif // ADAPTIVESUMRADIALFIELDMAP_HH