#include "greedy.h"


OptimizationResults greedy3(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 3 minimization -------------" << std::endl << std::endl;
    const uint gwCount = l->gwCount;
    const uint edCount = l->edCount;

    bool feasibleFound = false;    
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    
    if(verbose) std::cout << "Running " << iters << " iterations..." << std::endl << std::endl;

    for(uint iter = 0; iter < iters; iter++){
        uint gw[edCount];
        uint sf[edCount];
        std::vector<uint>gwList(gwCount);
        for(uint i = 0; i < gwCount; i++)
            gwList[i] = i;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(gwList.begin(), gwList.end(), gen);

        std::vector<uint>allocatedEDs;
        for(uint gi = 0; gi < gwList.size(); gi++){ // Allocate end devices
            const uint g = gwList[gi];        
            UtilizationFactor guf;
            for(uint s = 7; s <= 12; s++){ // For each spread factor        
                std::vector<uint> edList = l->getEDList(g, s);                
                for(uint e = 0; e < edList.size(); e++){
                    uint selectedED = edList[e];
                    if(std::find(allocatedEDs.begin(), allocatedEDs.end(), selectedED) == allocatedEDs.end()){ // If not allocated                    
                        UtilizationFactor u = l->getUF(selectedED, s);
                        if(!(guf + u).isFull()){ // If allowed to allocate (enough uf)
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
                    if(verbose){
                        std::cout << "New best at iteration: " << iter << std::endl;
                        o->printSolution(gw, sf, false);
                        std::cout << std::endl << std::endl;
                    }
                }
                //if(verbose) std::cout << "SF: " << s << " -- " << (feasible?"Feasible":"Unfeasible") << "  Cost = " << cost << std::endl;
                allocatedEDs.clear(); // Clear list of allocated and continue with next SFs
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
        if (elapsedSeconds >= timeout) {
            if(verbose) std::cout << "Time limit reached." << std::endl;
            break;
        }
    }

    if(wst) o->exportWST(gwBest, sfBest);

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
        std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
        if(feasibleFound){
            std::cout << "Best result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    return results;
}