//
//  ToricLattice.hpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/24/20.
//

#ifndef ToricLattice_h
#define ToricLattice_h

class ToricLattice : public Lattice {
private:
    std::vector<int> xValues;
    std::vector<int> yValues;
    std::vector<int> match;
    int count = 0;
    std::vector<std::vector<int>> syndrome;
    
    int wrappedDist(int x1, int y1, int x2, int y2);
    void getMatching();
    std::pair<int, int> preParityCheck();
    std::pair<int, int> matchParityCheck();
    
public:
    ToricLattice(int l);
    
    void flip(int i) override;
    
    bool checkErrors() override;
    
    void printSyndrome();
    
    friend std::ostream & operator<<(std::ostream & _stream, ToricLattice const & lattice);
};


#endif /* ToricLattice_h */
