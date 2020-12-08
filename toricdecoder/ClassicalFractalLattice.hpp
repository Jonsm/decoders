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
    std::vector<std::list<int>> fractalSyms;
    std::vector<std::set<int>> syndromes;
    std::vector<std::pair<int,int>> flipSyndrome;
    std::vector<PerfectMatching*> matchings;
    std::unordered_map<int, int> clusters;
    
    void fillRow(std::vector<int>* lastRow, std::vector<int>* currentRow);
    int wrappedDist(int i1, int i2);
    void getMatching();
    void getClusters();
    
public:
    ClassicalFractalLattice(int l);
    
    void flip(int i) override;
    
    bool checkErrors() override;
    
    void printSyndrome();
    
    friend std::ostream & operator<<(std::ostream & _stream, ClassicalFractalLattice const & lattice);
};

#endif /* ClassicalFractalLattice_hpp */
