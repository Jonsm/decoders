//
//  main.cpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/11/20.
//

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include "blossom5/PerfectMatching.h"
#include "GeneralLattice.hpp"
#include "ToricLattice.hpp"
#include "ClassicalFractalLattice.hpp"

using namespace std;

float pAccept(int oldFlips, int newFlips, float pError) {
    int diff = newFlips - oldFlips;
    if (diff == 1) {
        return pError / (1.0f - pError);
    } else {
        return 1;
    }
}

int mcSteps(Lattice& lattice, int nFlips, int nSteps, float pError) {
    int currentFlips = nFlips;
    int nSites = (int)lattice.errors.size();
    int i;
    int currentSpin;
    int newFlips;
    float accept;
    
    for (int j = 0; j < nSteps; j++) {
        i = rand() % nSites;
        currentSpin = lattice.errors[i];
        newFlips = currentFlips + (1 - 2 * currentSpin);
        accept = pAccept(currentFlips, newFlips, pError);
        if (accept == 1 || ((double) rand() / (RAND_MAX)) < accept) {
            lattice.flip(i);
            currentFlips = newFlips;
        }
    }
    
    return currentFlips;
}

template <class L>
float mcSimulation(int l, int tAnneal, int tSample, int nSamples, float pError) {
    srand(time(NULL));
    L lattice(l);
    int nFlips = mcSteps(lattice, 0, tAnneal, pError);
    int nSuccess = 0;
    for (int i = 0; i < nSamples; i++) {
        nFlips = mcSteps(lattice, nFlips, tSample, pError);
        if (lattice.checkErrors()) {
            nSuccess++;
        }
    }
    
    return ((float)nSuccess)/nSamples;
}

template<class L>
void writeSimToFile(vector<int> ls, vector<float> pErrors, int nSamples, int annealFactor, int sampleFactor, string filename) {
    ofstream myfile;
    myfile.open(filename);
    int tAnneal;
    int tSample;
    
    myfile << " ";
    
    for (float pError : pErrors) {
        myfile << "," << pError;
    }
    
    myfile << "\n";
    
    for (int l : ls) {
        tAnneal = annealFactor * l * l;
        tSample = sampleFactor * l * l;
        myfile << l;
        for (float pError : pErrors) {
            cout << l << " " << pError << "\n";
            float success = mcSimulation<L>(l,tAnneal, tSample, nSamples, pError);
            myfile << "," << success;
        }
        myfile << "\n";
    }
    
    myfile.close();
}

int main(int argc, const char * argv[]) {
//    ClassicalFractalLattice lattice(8);
//    lattice.flip(8);
//    lattice.flip(6);
//    lattice.flip(10);
//    lattice.flip(24);
//    lattice.flip(23);
////    lattice.flip(25);
//    lattice.flip(40);
//    lattice.flip(81);
//    lattice.flip(65);
//    lattice.flip(64);
//    lattice.flip(66);
    
//    lattice.flip(8);
//    lattice.flip(24);
//    lattice.flip(40);
//    lattice.flip(56);
//    lattice.flip(7);
//    lattice.flip(23);
//    lattice.flip(39);
//    lattice.flip(55);
//    lattice.flip(80);
//    lattice.flip(64);
//    lattice.flip(48);
    
//    cout << lattice;
//    cout << "------------\n";
//    lattice.printSyndrome();
//    cout << lattice.checkErrors();
//    cout << "------------\n";
//    lattice.printCorrection();
    
    /*********MAIN CODE******/
    float pMin = .05;
    float pMax = .2;
    int divs = 7;
    int nSamples = 1000;
    int annealFactor = 20;
    int sampleFactor = 3;
    string filename = "test.csv";

    vector<int> ls{4,8,16};
    vector<float> pErrors(divs);
    for (int i = 0 ; i < divs; i++) {
        pErrors[i] = pMin + ((pMax - pMin) * i) / (divs - 1);
    }

    auto t1 = chrono::high_resolution_clock::now();
    writeSimToFile<ClassicalFractalLattice>(ls, pErrors, nSamples, annealFactor, sampleFactor, filename);
    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double> dur = duration_cast<chrono::duration<double>>(t2 -t1);
    cout << dur.count() << "\n";
}
