//
//  ClassicalFractalLattice.cpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/24/20.
//
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include "GeneralLattice.hpp"
#include "blossom5/PerfectMatching.h"
#include "UnionFind.hpp"
#include "ClassicalFractalLattice.hpp"

using namespace std;

void ClassicalFractalLattice::fillRow(vector<int>* lastRow, vector<int>* currentRow) {
    for (int x = 0; x < 2*l; x++) {
        (*currentRow)[x] = ((*lastRow)[x] + (*lastRow)[(x-1 + 2*l) % (2*l)] + (*lastRow)[(x+1) % (2*l)]) % 2;
    }
}

ClassicalFractalLattice::ClassicalFractalLattice(int l) : Lattice(l) {
    errors = vector<int>(2*l*l);
    syndromes = vector<set<int>>(2*l);
    matchings = vector<PerfectMatching*>(2*l);
    fractalSyms = vector<list<int>>(2*l*l);
    flipSyndrome = vector<pair<int,int>>{make_pair(0,0),make_pair(-1,-1),make_pair(1,-1),make_pair(0,-1)};
    
    vector<int>* currentRow = new vector<int>(2*l);
    vector<int>* lastRow = new vector<int>(2*l);
    (*currentRow)[0] = 1;
    for (int y = 0; y < l; y++) {
        for (int x = 0; x < 2*l; x++) {
            if ((*currentRow)[x] == 1) {
                for (int x2 = 0; x2 < 2*l; x2++) {
                    int ind = y*2*l + (x + x2) % (2*l);
                    fractalSyms[ind].push_back(x2);
                }
            }
        }
        
        vector<int>* tmpRow = lastRow;
        lastRow = currentRow;
        currentRow = tmpRow;
        fill((*currentRow).begin(), (*currentRow).end(), 0);
        fillRow(lastRow, currentRow);
    }
    
    delete currentRow;
    delete lastRow;
}

void ClassicalFractalLattice::flip(int i) {
    int x = i % (2*l);
    int y = i / (2*l);
    for (pair<int,int> coord : flipSyndrome) {
        int synX = (x + coord.first + 2*l) % (2*l);
        int synY = (y + coord.second + l) % l;
        int ind = synY * 2 * l + synX;
        set<int>& syndromesFirst = syndromes[fractalSyms[ind].front()];
        
        if (syndromesFirst.find(ind) == syndromesFirst.end()) { //bad
            for (int sym : fractalSyms[ind]) {
                syndromes[sym].insert(ind);
            }
        } else {
            for (int sym : fractalSyms[ind]) {
                syndromes[sym].erase(ind);
            }
        }
    }
    errors[x + 2*l*y] = (errors[x + 2*l*y] + 1) % 2;
}

int ClassicalFractalLattice::wrappedDist(int i1, int i2) {
    int x1 = i1 % (2*l);
    int x2 = i2 % (2*l);
    int y1 = i1 / (2*l);
    int y2 = i2 / (2*l);
    int x_dist = min(abs(x1-x2),l-abs(x1-x2));
    int y_dist = min(abs(y1-y2),l-abs(y1-y2));
    return x_dist + y_dist;
}

void ClassicalFractalLattice::getMatching() {
    for (int sym = 0; sym < 2*l; sym++) {
        int count = (int)syndromes[sym].size();
        if (count == 0) continue;
        if (matchings[sym] != nullptr) {
            delete matchings[sym];
        }
        matchings[sym] = new PerfectMatching(count, count*(count - 1)/2);
        matchings[sym]->options.verbose = false;
        
        set<int>::iterator i = syndromes[sym].begin();
        set<int>::iterator j;
        int iind = 0;
        int jind = 0;
        
        while (i != syndromes[sym].end()) {
            j = i;
            j++;
            jind = iind + 1;
            while (j != syndromes[sym].end()) {
                int src = *i;
                int dest = *j;
                int dist = wrappedDist(src, dest);
                matchings[sym]->AddEdge(iind, jind, dist);
                j++;
                jind++;
            }
            i++;
            iind++;
            matchings[sym]->Solve();
        }
    }
}

void ClassicalFractalLattice::getClusters() {
    set<int> allErrors;
    
    for (int sym = 0; sym < 2*l; sym++) {
        if (matchings[sym]==nullptr) continue;
        cout << sym << ":\n";
        set<int>::iterator i = syndromes[sym].begin();
        int iind = 0;
        
        while (i != syndromes[sym].end()) {
            
            i++;
            iind++;
        }
//        for (int i : syndromes[sym]) {
//            cout << i << " " << matchings[sym]->GetMatch(i) << "\n";
//        }
    }
}

bool ClassicalFractalLattice::checkErrors() {
    getMatching();
    getClusters();
    return false;
}

void ClassicalFractalLattice::printSyndrome() {
    for (int i = 0; i < 2*l; i++) {
        cout << i <<": ";
        for (int ind : syndromes[i]) {
            cout << ind << ", ";
        }
        cout << "\n";
    }
}

std::ostream & operator<<(std::ostream & _stream, ClassicalFractalLattice const & lattice) {
    int l = lattice.l;
    string latString = "";
    for (int y = 0; y < l; y++) {
        string lineString = "";
        for (int x = 0; x < 2*l; x++) {
            lineString += to_string(lattice.errors[y*2*l + x]) + " ";
        }
        latString += lineString + "\n";
    }
    return _stream << latString;
}
