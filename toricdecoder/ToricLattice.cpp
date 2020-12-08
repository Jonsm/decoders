//
//  ToricLattice.cpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/24/20.
//
#include <vector>
#include <iostream>
#include "GeneralLattice.hpp"
#include "blossom5/PerfectMatching.h"
#include "ToricLattice.hpp"

using namespace std;

int ToricLattice::wrappedDist(int x1, int y1, int x2, int y2) {
    int x_dist = min(abs(x1-x2),l-abs(x1-x2));
    int y_dist = min(abs(y1-y2),l-abs(y1-y2));
    return x_dist + y_dist;
}
    
void ToricLattice::getMatching() {
    count = 0;
    for (int x = 0; x < l; x++) {
        for (int y = 0; y < l; y++) {
            if (syndrome[x][y] == 1) {
                xValues[count] = x;
                yValues[count] = y;
                count++;
            }
        }
    }

    PerfectMatching pm(count, count*(count-1)/2);
    pm.options.verbose = false;
    
    for (int src = 0; src < count; src++) {
        for (int dest = src + 1; dest < count; dest++) {
            int dist = wrappedDist(xValues[src], yValues[src], xValues[dest], yValues[dest]);
            pm.AddEdge(src, dest, dist);
        }
    }
    pm.Solve();
    for (int i = 0; i < count; i++) {
        match[i] = pm.GetMatch(i);
    }
}

pair<int, int> ToricLattice::preParityCheck() {
    pair<int, int> parity;
    
    for (int x = 0; x < l; x++) {
        parity.first += errors[l*l+(l-1)*l+x];
    }
    
    for (int y = 0; y < l; y++) {
        parity.second += errors[(l-1)*l+y];
    }
    
    return parity;
}

pair<int, int> ToricLattice::matchParityCheck() {
    pair<int, int> parity;
    
    int matchV;
    for (int i = 0; i < count; i++) {
        if (i > match[i]) {
            continue;
        }
        
        matchV = match[i];
        if (abs(yValues[matchV] - yValues[i]) > l / 2) {
            parity.first++;
        }
        if (abs(xValues[matchV] - xValues[i]) > l / 2) {
            parity.second++;
        }
    }
    
    return parity;
}

ToricLattice::ToricLattice(int l) : Lattice(l) {
    errors = vector<int>(2*l*l);
    xValues = vector<int>(l*l);
    yValues = vector<int>(l*l);
    syndrome = vector<vector<int>>(l);
    match = vector<int>(l*l);
    for (int i = 0; i < l; i++) {
        syndrome[i].resize(l);
    }
}

void ToricLattice::flip(int i) {
    errors[i] = (errors[i]) ^ 1;
    if (i < l*l) {
        int xLeft = i / l;
        int xRight = (xLeft + 1) % l;
        int y = i % l;
        syndrome[xLeft][y] = (syndrome[xLeft][y]) ^ 1;
        syndrome[xRight][y] = (syndrome[xRight][y]) ^ 1;
    } else {
        int x = (i - l*l) % l;
        int yTop = (i - l*l) / l;
        int yBot = (yTop + 1) % l;
        syndrome[x][yTop] = (syndrome[x][yTop]) ^ 1;
        syndrome[x][yBot] = (syndrome[x][yBot]) ^ 1;
    }
}

bool ToricLattice::checkErrors() {
    getMatching();
    pair<int, int> preParity = preParityCheck();
    pair<int, int> matchParity = matchParityCheck();
    return (preParity.first % 2 == matchParity.first % 2) &&
    (preParity.second % 2 == matchParity.second % 2);
}

void ToricLattice::printSyndrome() {
    cout << "\n";
    for (int y = 0; y < l; y++) {
        for (int x = 0; x < l; x++) {
            cout << syndrome[x][y] << " ";
        }
        cout << "\n";
    }
}
    
std::ostream & operator<<(std::ostream & _stream, ToricLattice const & lattice) {
    int l = lattice.l;
    string latString = "";
        for (int i = 0; i < l; i++) {
            string lineString = "  ";
            if (i == 0) {
                lineString = "\n  ";
                for (int j = 0; j < l; j++) {
                    lineString += "| ";
                }
                lineString += "\n  ";
            }

            for (int j = 0; j < l; j++) {
                lineString += to_string(lattice.errors[l*j+i]) + " ";
            }
            lineString += "\n-";

            for (int j = 0; j < l; j++) {
                lineString += to_string(lattice.errors[l*l + i*l + j]) + "+";
            }
            lineString += "\n";

            latString += lineString;
        }
    return _stream << latString;
}
