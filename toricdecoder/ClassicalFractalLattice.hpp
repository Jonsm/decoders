//
//  ClassicalFractalLattice.hpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/24/20.
//

#ifndef ClassicalFractalLattice_hpp
#define ClassicalFractalLattice_hpp

class ClassicalFractalLattice : public Lattice {
private:
    struct CompByX {
        CompByX(const ClassicalFractalLattice& lattice);
        const ClassicalFractalLattice& lattice;
        
        bool operator() (const int & i1, const int & i2);
    };
    
    struct CompByY {
        CompByY(const ClassicalFractalLattice& lattice);
        const ClassicalFractalLattice& lattice;
        
        bool operator() (const int & i1, const int & i2);
    };
    
    std::vector<std::list<int>> fractalSyms;
    std::vector<std::vector<int>> syndromesSyms;
    std::set<int> syndromes;
    std::vector<std::pair<int,int>> flipSyndrome;
    std::vector<PerfectMatching*> matchings;
    std::unordered_map<int, std::vector<int>> clusters;
    std::set<int> clusterIds;
    std::vector<int> correctionOperator;
    
    void fillRow(std::vector<int>* lastRow, std::vector<int>* currentRow);
    
    int wrappedDist(int i1, int i2);
    
    void getMatching();
    
    void getClusters();
    
    void correctCluster(int clusterId);
    
    void correctClusterTall(std::vector<int>& xSort, int pos);
    
    void correctClusterWide(std::vector<int>& ySort, int pos);
    
    void getDims(std::vector<int>& xSort, std::vector<int>& ySort, std::pair<int,int>& dims, std::pair<int,int>& pos);
    
    std::string printHelper(const std::vector<int>& arr) const;
    
public:
    ClassicalFractalLattice(int l);
    
    void flip(int i) override;
    
    bool checkErrors() override;
    
    void printSyndrome();
    
    void printCorrection();
    
    friend std::ostream & operator<<(std::ostream & _stream, ClassicalFractalLattice const & lattice);
};

#endif /* ClassicalFractalLattice_hpp */
