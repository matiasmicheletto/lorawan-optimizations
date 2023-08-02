#include "greedy.h"

OptimizationResults greedy2(Instance* l, Objective* o, bool verbose){
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "------------- Greedy 2 minimization -------------" << std::endl << std::endl;
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    // Allocate optimization variable (initialization should not be required)
    uint* gw = (uint*) malloc( sizeof(uint) * edCount);
    uint* sf = (uint*) malloc( sizeof(uint) * edCount);

    // STEP 1
    std::vector<std::vector<uint>> ady;
    for(uint s = 7; s <= 12; s++){
        std::vector<uint> g(gwCount);
        ady.push_back(g);
        std::cout << "SF:" << s << "{ ";
        for(uint col = 0; col < gwCount; col++){
            for(uint row = 0; row < edCount; row++){
                if(l->getMinSF(row, col) <= s)
                    ady[s-7][col]++; // Adjacent counter for each sf and gw
            }
            std::cout << ady[s-7][col] << " ";
        }
        std::cout << "}" << std::endl;
    }

    // STEP 2 -- todo
    for(uint e = 0; e < edCount; e++){
        gw[e] = 0;
        sf[e] = 11;
    }

    if(verbose){
        std::cout << "Result:" << std::endl;
        o->printSolution(gw, sf, true, true);
    }

    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults results;
    results.cost = o->eval(gw, sf, results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    // Release memory used for optimization variable
    free(gw);
    free(sf);

    return results;
}