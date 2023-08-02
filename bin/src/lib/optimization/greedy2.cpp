#include "greedy.h"

OptimizationResults greedy2(Instance* l, Objective* o, bool verbose){
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "------------- Greedy 2 minimization -------------" << std::endl << std::endl;
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    // STEP 1
    if(verbose) std::cout << "Step 1: Adjacency list (nodes for each gw)" << std::endl;
    std::vector<std::vector<uint>> ady;
    for(uint s = 7; s <= 12; s++){
        std::vector<uint> g(gwCount);
        ady.push_back(g);
        if(verbose) std::cout << "SF: " << s << "  { ";
        for(uint col = 0; col < gwCount; col++){
            for(uint row = 0; row < edCount; row++){
                if(l->getMinSF(row, col) <= s)
                    ady[s-7][col]++; // Adjacent counter for each sf and gw
            }
            if(verbose) std::cout << ady[s-7][col] << " ";
        }
        if(verbose) std::cout << "}" << std::endl;
    }

    std::cout << std::endl << "Step 2: SF eval" << std::endl;

    // STEP 2 
    uint gw[edCount];
    uint sf[edCount];
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    for(uint s = 7; s <= 12; s++){ // For each spread factor
        for(uint e = 0; e < edCount; e++){ // Allocate end devices
            std::vector<uint> gwList = l->getSortedGWList(e); // Valid gw for this ed
            gw[e] = gwList[0];
            sf[e] = s;
        }
        // Eval solution and update minimum
        uint gwUsed, energy; double uf; bool feasible;
        const double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
        if(feasible && cost < minimumCost){ // New optimum
            minimumCost = cost;
            std::copy(gw, gw + edCount, gwBest);
            std::copy(sf, sf + edCount, sfBest);
        }
        if(verbose) std::cout << "SF: " << s << " -- " << (feasible?"Feasible":"Unfeasible") << "  Cost = " << cost << std::endl;
    }

    if(verbose){
        std::cout << "Result:" << std::endl;
        o->printSolution(gwBest, sfBest, true, true);
    }
    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults results;
    results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready


    return results;
}