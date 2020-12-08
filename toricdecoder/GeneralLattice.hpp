//
//  GeneralLattice.hpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/24/20.
//

#ifndef GeneralLattice_h
#define GeneralLattice_h

class Lattice {
public:
    int l;
    std::vector<int> errors;
    
    virtual void flip(int i) = 0;
    virtual bool checkErrors() = 0;
    
    Lattice(int l) : l(l) {
        
    }
};

#endif /* GeneralLattice_h */
