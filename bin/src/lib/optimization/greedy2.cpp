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
    bool feasibleFound = false;
    std::vector<uint>allocatedEDs;
    
    for(uint g = 0; g < gwCount; g++){ // Allocate end devices
        double guf = 0.0; // GW utilization factor
        for(uint s = 7; s <= 12; s++){ // For each spread factor        
            double pw = (double) l->sf2e(s);    
            std::vector<uint> edList = l->getEDList(g, s);                
            for(uint e = 0; e < edList.size(); e++){
                uint selectedED = edList[e];
                if(std::find(allocatedEDs.begin(), allocatedEDs.end(), selectedED) == allocatedEDs.end()){ // If not allocated
                    double u = pw / ((double)l->getPeriod(selectedED) - pw);
                    if(guf + u < 1){ // If allowed to allocate (enough uf)
                        allocatedEDs.push_back(selectedED);
                        gw[selectedED] = g;
                        sf[selectedED] = s;
                        guf += u;
                    }
                }
                if(allocatedEDs.size() == edCount) break;
            }
            if(allocatedEDs.size() == edCount) break;
        }
        if(allocatedEDs.size() == edCount){
            // Eval solution and update minimum
            uint gwUsed, energy; double uf; bool feasible;
            const double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
            if(feasible && cost < minimumCost){ // New optimum
                minimumCost = cost;
                std::copy(gw, gw + edCount, gwBest);
                std::copy(sf, sf + edCount, sfBest);
                feasibleFound = true;
            }
            //if(verbose) std::cout << "SF: " << s << " -- " << (feasible?"Feasible":"Unfeasible") << "  Cost = " << cost << std::endl;
            allocatedEDs.clear(); // Clear list of allocated and continue with next SFs
        }
    }

    if(verbose){
        if(feasibleFound){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    return results;
}