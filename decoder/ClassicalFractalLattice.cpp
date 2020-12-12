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

ClassicalFractalLattice::CompByX::CompByX(const ClassicalFractalLattice &lattice) :
lattice(lattice) { }

bool ClassicalFractalLattice::CompByX::operator() (const int & i1, const int & i2) {
    return (i1 % (2*lattice.l)) < (i2 % (2*lattice.l));
}

ClassicalFractalLattice::CompByY::CompByY(const ClassicalFractalLattice &lattice) :
lattice(lattice) { }

bool ClassicalFractalLattice::CompByY::operator() (const int & i1, const int & i2) {
    return (i1 / (2*lattice.l)) < (i2 / (2*lattice.l));
}

void ClassicalFractalLattice::fillRow(vector<int>* lastRow, vector<int>* currentRow) {
    for (int x = 0; x < 2*l; x++) {
        (*currentRow)[x] = ((*lastRow)[x] + (*lastRow)[(x-1 + 2*l) % (2*l)] + (*lastRow)[(x+1) % (2*l)]) % 2;
    }
}

ClassicalFractalLattice::ClassicalFractalLattice(int l) : Lattice(l) {
    errors = vector<int>(2*l*l);
    correctionOperator = vector<int>(2*l*l);
    syndromesSyms = vector<vector<int>>(2*l);
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
        if (syndromes.find(ind) == syndromes.end()) {
            syndromes.insert(ind);
        } else {
            syndromes.erase(ind);
        }
    }
    errors[x + 2*l*y] = (errors[x + 2*l*y] + 1) % 2;
}

int ClassicalFractalLattice::wrappedDist(int i1, int i2) {
    int x1 = i1 % (2*l);
    int x2 = i2 % (2*l);
    int y1 = i1 / (2*l);
    int y2 = i2 / (2*l);
    int x_dist = min(abs(x1-x2),2*l-abs(x1-x2));
    int y_dist = min(abs(y1-y2),l-abs(y1-y2));
    return x_dist + y_dist;
}

void ClassicalFractalLattice::getMatching() {
    for (int sym = 0; sym < 2*l; sym++) {
        syndromesSyms[sym].clear();
    }
    
    for (int ind : syndromes) {
        for (int sym : fractalSyms[ind]) {
            syndromesSyms[sym].push_back(ind);
        }
    }
    
    for (int sym = 0; sym < 2*l; sym++) {
        int count = (int)syndromesSyms[sym].size();
        if (matchings[sym] != nullptr) {
            delete matchings[sym];
            matchings[sym] = nullptr;
        }
        if (count == 0) continue;
        
        matchings[sym] = new PerfectMatching(count, count*(count - 1)/2);
        matchings[sym]->options.verbose = false;

        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                int src = syndromesSyms[sym][i];
                int dest = syndromesSyms[sym][j];
                int dist = wrappedDist(src, dest);
                matchings[sym]->AddEdge(i,j,dist);
            }
        }
        matchings[sym]->Solve();
    }
}

void ClassicalFractalLattice::getClusters() {
    UF uf(2*l*l);
    for (int sym = 0; sym < 2*l; sym++) {
        if (matchings[sym]==nullptr) continue;
        for (int i = 0; i < syndromesSyms[sym].size(); i++) {
            int src = syndromesSyms[sym][i];
            int dest = syndromesSyms[sym][matchings[sym]->GetMatch(i)];
            uf.merge(src, dest);
        }
    }

    clusters.clear();
    clusterIds.clear();
    for (int ind : syndromes) {
        int id = uf.find(ind);
        if (clusters.find(id) == clusters.end()) {
            clusters[id] = vector<int>();
        }
        clusters[id].push_back(ind);
        clusterIds.insert(id);
    }
}

void ClassicalFractalLattice::getDims(std::vector<int> &xSort,std::vector<int> &ySort, std::pair<int, int>& dims, std::pair<int,int>& pos) {
    dims.first = 2*l;
    dims.second = l;
    
    for (int i = 0; i < xSort.size(); i++) {
        int next = (i + 1) % xSort.size();
        int xGap = (xSort[next] % (2*l) - xSort[i] % (2*l) + 2*l) % (2*l);
        int yGap = (ySort[next] / (2*l) - ySort[i] / (2*l) + l) % l;
        if (2*l - xGap + 1 < dims.first) {
            dims.first = 2*l - xGap + 1;
            pos.first = i;
        }
        if (l - yGap + 1 < dims.second) {
            dims.second = l - yGap + 1;
            pos.second = i;
        }
    }
}

void ClassicalFractalLattice::correctCluster(int clusterId) {
    vector<int> ySort = clusters[clusterId];
    vector<int> xSort = clusters[clusterId];
    sort(xSort.begin(), xSort.end(), CompByX(*this));
    sort(ySort.begin(), ySort.end(), CompByY(*this));
    std::pair<int, int> dims, pos;
    
    getDims(xSort, ySort, dims, pos);
    if (dims.first > dims.second && dims.second < l) {
        correctClusterWide(ySort, pos.second);
    } else {
        correctClusterTall(xSort, pos.first);
    }
}

void ClassicalFractalLattice::correctClusterTall(std::vector<int> &xSort, int pos) {
    int currentX = xSort[pos] % (2*l);
    int currentPos = pos;
    int endX = xSort[(pos + 1) % xSort.size()] % (2*l);
    vector<int>* nextCol = new vector<int>(l);
    vector<int>* currentCol = new vector<int>(l);
    vector<int>* lastCol = new vector<int>(l);
    
    do {
        vector<int>* tmp = lastCol;
        lastCol = currentCol;
        currentCol = nextCol;
        nextCol = tmp;
        fill(nextCol->begin(),nextCol->end(), 0);
        
        for (int y = 0; y < l; y++) {
            int ind = y * (2*l) + currentX;
            correctionOperator[ind] = (correctionOperator[ind] + (*lastCol)[(y - 1 + l) % l]) % 2;
            (*currentCol)[y] = ((*currentCol)[y] + (*lastCol)[y] + (*lastCol)[(y-1 + l) % l]) % 2;
            (*nextCol)[y] = ((*nextCol)[y] + (*lastCol)[y]) % 2;
        }
        
        while (xSort[currentPos] % (2*l) == currentX) {
            int y = xSort[currentPos] / (2*l);
            (*currentCol)[y] = ((*currentCol)[y] + 1) % 2;
            currentPos = (currentPos - 1 + (int)xSort.size()) % xSort.size();
        }
        
        currentX = (currentX - 1 + 2*l) % (2*l);
    } while (currentX != endX);
    
    delete nextCol;
    delete currentCol;
    delete lastCol;
}

void ClassicalFractalLattice::correctClusterWide(std::vector<int> &ySort, int pos) {
    int currentY = ySort[pos] / (2*l);
    int currentPos = pos;
    int endY = ySort[(pos + 1) % ySort.size()] / (2*l);
    vector<int>* currentRow = new vector<int>(2*l);
    vector<int>* lastRow = new vector<int>(2*l);

    do {
        vector<int>* tmp = lastRow;
        lastRow = currentRow;
        fill(tmp->begin(),tmp->end(), 0);
        currentRow = tmp;
        fillRow(lastRow, currentRow);
        
        while (ySort[currentPos] / (2*l) == currentY) {
            int x = ySort[currentPos] % (2*l);
            (*currentRow)[x] = ((*currentRow)[x] + 1) % 2;
            currentPos = (currentPos - 1 + (int)ySort.size()) % ySort.size();
        }
        
        for (int x = 0; x < 2*l; x++) {
            int ind = currentY * 2 * l + x;
            correctionOperator[ind] = (correctionOperator[ind] + (*currentRow)[x]) % 2;
        }

        currentY = (currentY - 1 + l) % l;
    } while (currentY != endY);
    
    delete currentRow;
    delete lastRow;
}

bool ClassicalFractalLattice::checkErrors() {
    if (syndromes.size() == 0) return true;
    getMatching();
    getClusters();
    
    fill(correctionOperator.begin(), correctionOperator.end(), 0);
    
    for (int id : clusterIds) {
        correctCluster(id);
    }
    
    for (int i = 0; i < 2*l*l; i++) {
        if (correctionOperator[i] != errors[i]) return false;
    }
    
    return true;
}

void ClassicalFractalLattice::printSyndrome() {
    for (int y = 0; y < l; y++) {
        string lineString = "";
        for (int x = 0; x < 2 * l; x++) {
            int ind = y*2*l + x;
            string indSyn;
            if (syndromes.find(ind) == syndromes.end()) {
                indSyn = "0";
            } else {
                indSyn = "1";
            }
            lineString += indSyn + " ";
        }
        cout << lineString << "\n";
    }
}

void ClassicalFractalLattice::printCorrection() {
    cout << printHelper(correctionOperator);
}

string ClassicalFractalLattice::printHelper(const std::vector<int>& arr) const {
    string latString = "";
    for (int y = 0; y < l; y++) {
        string lineString = "";
        for (int x = 0; x < 2*l; x++) {
            lineString += to_string(arr[y*2*l + x]) + " ";
        }
        latString += lineString + "\n";
    }
    
    return latString;
}

std::ostream & operator<<(std::ostream & _stream, ClassicalFractalLattice const & lattice) {
    return _stream << lattice.printHelper(lattice.errors);
}
