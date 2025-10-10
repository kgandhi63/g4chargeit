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
#include <fftw3.h> // Include the FFTW library

class AdaptiveSumRadialFieldMap : public G4ElectricField {
public:
    enum class StorageType { Float, Double };

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

    ~AdaptiveSumRadialFieldMap() override; // Must now have a destructor to clean up FFTW plans
    void GetFieldValue(const G4double point[4], G4double field[6]) const override;
    void ExportFieldMapToFile(const std::string& filename) const;
    void PrintMeshStatistics() const;

private:
    std::unique_ptr<Node> root_;
    int max_depth_;
    
    std::vector<G4ThreeVector> fPositions;
    std::vector<G4double> fCharges;
    G4double worldX_, worldY_, worldZ_;
    G4ThreeVector worldMin_, worldMax_;
    G4double minStepSize_;
    StorageType fStorage;

    // --- PIC Grid Members ---
    int fNx_, fNy_, fNz_;
    G4ThreeVector fStep_;
    std::vector<double> charge_grid_;
    std::vector<double> potential_grid_; // NEW: Grid for electric potential
    std::vector<G4ThreeVector> field_grid_;

    // --- FFTW Members ---
    fftw_plan fft_plan_forward_;
    fftw_plan fft_plan_backward_;
    fftw_complex* fft_charge_grid_;
    fftw_complex* fft_potential_grid_;

    std::vector<Node*> all_leaves_;

    double fieldGradThreshold_;
    mutable size_t total_nodes_, leaf_nodes_, gradient_refinements_;
    mutable int max_depth_reached_;

    // Field Map construction methods
    std::unique_ptr<Node> buildFromScratch();
    std::unique_ptr<Node> createOctreeFromScratch(const G4ThreeVector& min, const G4ThreeVector& max, int depth);
    void refineMeshByGradient(Node* node, int depth);
    
    // PIC methods
    void initializePICGrid(int nx, int ny, int nz);
    void depositCharges();
    void solveFieldOnGrid(); // This will be rewritten for FFT
    G4ThreeVector interpolateFieldFromGrid(const G4ThreeVector& point) const;

    G4ThreeVector computeFieldFromCharges(const G4ThreeVector& point) const;

    bool hasHighFieldGradient(const G4ThreeVector& center, const G4ThreeVector& center_field, double sample_distance) const;
    G4ThreeVector evaluateField(const G4ThreeVector& point, const Node* node) const;
    
    // Utility functions
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

