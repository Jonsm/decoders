//
//  main.cpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/11/20.
//

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/maximum_weighted_matching.hpp>

using namespace std;
using namespace boost;

typedef property< edge_weight_t, int, property< edge_index_t, int > >
    EdgeProperty;
//typedef adjacency_matrix< undirectedS, no_property, EdgeProperty >
//    Graph;
typedef adjacency_list< vecS, vecS, undirectedS, no_property, EdgeProperty >
    Graph;
typedef graph_traits< Graph >::vertex_descriptor Vertex;

class Lattice {
private:
    vector<int> xValues;
    vector<int> yValues;
    Graph g;
    vector< graph_traits< Graph >::vertex_descriptor > match;
    
public:
    int l;
    vector<int> errors;
    vector<vector<int>> syndrome;
    
    Lattice(int l) : l(l), g(0) {
        errors = vector<int>(2*l*l);
        xValues = vector<int>(l*l);
        yValues = vector<int>(l*l);
        syndrome = vector<vector<int>>(l);
//        match = vector< graph_traits< Graph >::vertex_descriptor > (l*l);
//        g = Graph(0);
        for (int i = 0; i < l; i++) {
            syndrome[i].resize(l);
        }
    }
    
    void flip(int i) {
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
    
    int wrappedDist(int x1, int y1, int x2, int y2) {
        int x_dist = min(abs(x1-x2),l-abs(x1-x2));
        int y_dist = min(abs(y1-y2),l-abs(y1-y2));
        return x_dist + y_dist;
    }
    
    void getMatching() {
//        g.clear();
        int count = 0;
        for (int x = 0; x < l; x++) {
            for (int y = 0; y < l; y++) {
                if (syndrome[x][y] == 1) {
                    xValues[count] = x;
                    yValues[count] = y;
                    count++;
//                    add_vertex(g);
                }
            }
        }
        g = Graph(count);

        for (int src = 0; src < count; src++) {
            for (int dest = src + 1; dest < count; dest++) {
                int dist = wrappedDist(xValues[src], yValues[src], xValues[dest], yValues[dest]);
                add_edge(src,dest,EdgeProperty(l*l - dist),g);
                cout << src << " " << dest << " " << dist << "\n";
            }
        }

//        if (l == 5) {
////            cout << count << "\n";
//            cout << "--------\n";
//            printSyndrome();
//        }
        
        match = vector< graph_traits< Graph >::vertex_descriptor > (200);
//        std::map<Vertex, Vertex> match2;
//        boost::associative_property_map< std::map<Vertex,Vertex> > mapAdapter(match2);
        maximum_weighted_matching(g, &match[0]);
    }
    
    pair<int, int> preParityCheck() {
        pair<int, int> parity;
        
        for (int x = 0; x < l; x++) {
            parity.first += errors[l*l+(l-1)*l+x];
        }
        
        for (int y = 0; y < l; y++) {
            parity.second += errors[(l-1)*l+y];
        }
        
        return parity;
    }
    
    pair<int, int> matchParityCheck() {
        pair<int, int> parity;
        
        graph_traits< Graph >::vertex_iterator vi, vi_end;
        int matchV;
        for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; vi++) {
//            if (*vi > match[*vi]) {
//                continue;
//            }
//            matchV = match[*vi];
//            if (abs(yValues[matchV] - yValues[*vi]) > l / 2) {
//                parity.first++;
//            }
//            if (abs(xValues[matchV] - xValues[*vi]) > l / 2) {
//                parity.second++;
//            }
        }
        
        return parity;
    }
    
    bool checkErrors() {
        getMatching();
        pair<int, int> preParity = preParityCheck();
        pair<int, int> matchParity = matchParityCheck();
//        cout << "pre " << preParity.first << " " << preParity.second << " match " <<matchParity.first << " " << matchParity.second << "\n";
        return (preParity.first % 2 == matchParity.first % 2) &&
        (preParity.second % 2 == matchParity.second % 2);
    }
    
    void printSyndrome() {
        cout << "\n";
        for (int y = 0; y < l; y++) {
            for (int x = 0; x < l; x++) {
                cout << syndrome[x][y] << " ";
            }
            cout << "\n";
        }
    }
    
    void printMatching() {
        graph_traits< Graph >::vertex_iterator vi, vi_end;
        std::cout << "The matching is:" << std::endl;
        for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
            if (match[*vi] != graph_traits< Graph >::null_vertex()
                && *vi < match[*vi])
                std::cout << "{" << *vi << ", " << match[*vi] << "}" << std::endl;
        std::cout << std::endl;
    }
    
    friend std::ostream & operator<<(std::ostream & _stream, Lattice const & lattice) {
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
};

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
    int nSites = 2 * lattice.l * lattice.l;
    int i;
    int currentSpin;
    int newFlips;
    float accept;
    
//    auto t1 = chrono::high_resolution_clock::now();
    
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
    
//    auto t2 = chrono::high_resolution_clock::now();
//
//    if (lattice.l == 9) {
//        chrono::duration<double> dur = duration_cast<chrono::duration<double>>(t2 -t1);
//        cout << dur.count() << "\n";
//    }
    
    return currentFlips;
}

float mcSimulation(int l, int tAnneal, int tSample, int nSamples, float pError) {
    srand(time(NULL));
    Lattice lattice(l);
    int nFlips = mcSteps(lattice, 0, tAnneal, pError);
    
    int nSuccess = 0;
    for (int i = 0; i < nSamples; i++) {
//        if (l == 9) {
//            cout << "-----" << i << "\n";
//        }
        nFlips = mcSteps(lattice, nFlips, tSample, pError);
        if (lattice.checkErrors()) {
            nSuccess++;
        }
    }
    
    return ((float)nSuccess)/nSamples;
}

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
            float success = mcSimulation(l,tAnneal, tSample, nSamples, pError);
            myfile << "," << success;
        }
        myfile << "\n";
    }
    
    myfile.close();
}

int main(int argc, const char * argv[]) {
    Lattice lattice = Lattice(5);


    lattice.flip(15);
    lattice.flip(34);
    lattice.flip(40);
    lattice.flip(42);
    lattice.flip(43);
    lattice.flip(44);
    lattice.flip(46);
    cout << lattice;
    lattice.printSyndrome();
    lattice.checkErrors();
//    lattice.printMatching();
    
    /*********MAIN CODE******/
//    float pMin = .09;
//    float pMax = .11;
//    int divs = 5;
//    int nSamples = 50000;
//    int annealFactor = 20;
//    int sampleFactor = 3;
//    string filename = "/Users/jon/Documents/Research/QI/Python/QEC/test.csv";
//
//    vector<int> ls{5};
//    vector<float> pErrors(divs);
//    for (int i = 0 ; i < divs; i++) {
//        pErrors[i] = pMin + ((pMax - pMin) * i) / (divs - 1);
//    }
//
//    auto t1 = chrono::high_resolution_clock::now();
//    writeSimToFile(ls, pErrors, nSamples, annealFactor, sampleFactor, filename);
//    auto t2 = chrono::high_resolution_clock::now();
//    chrono::duration<double> dur = duration_cast<chrono::duration<double>>(t2 -t1);
//    cout << dur.count() << "\n";
}
