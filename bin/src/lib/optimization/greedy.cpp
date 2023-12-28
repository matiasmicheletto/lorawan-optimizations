#include "greedy.h"


OptimizationResults greedy(Instance* l, Objective* o, uint iters, uint timeout){

    std::cout << "------------- Greedy minimization -------------" << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;
    
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();
    
    // Best
    uint gwBest[edCount];
    uint sfBest[edCount];

    bool feasibleFound = false;
    double minimumCost = __DBL_MAX__;
     
    // Essential gws
    std::cout << std::endl << "Stage 1 -- Find essential nodes" << std::endl;
    std::vector<uint> essGW;
    std::vector<uint> essED;
    for (uint e = 0; e < edCount; e++){
        auto it = std::find(essED.begin(), essED.end(), e);
        if(it == essED.end()){  // If not found
            const std::vector<uint> gwList = l->getGWList(e);
            if(gwList.size() == 1){
                const uint g = gwList[0];
                std::cout << "GW " << g << " is essential ";
                essGW.push_back(g);
                // Allocate all EDs of g (essential GW)
                const std::vector<uint> edList = l->getAllEDList(g, 12);
                const uint es = edList.size();
                std::cout << "and has " << es << " EDs: ";
                for(uint i = 0; i < es; i++){
                    essED.push_back(i);
                    std::cout << i << " ";
                    gwBest[i] = g;
                    sfBest[i] = l->getMinSF(i, g);
                }
                std::cout << std::endl;
            }
        }
    }
    if(essGW.size() == 0)
        std::cout << "No essential nodes found." << std::endl;


    ////// G4 //////
    std::cout << std::endl << "Stage 2 -- Allocation of non-essential nodes" << std::endl;

    std::vector<std::vector<std::vector<uint>>> clusters; // Clusters tensor (SF x GW x ED)
    clusters.resize(6); // Initialize list of matrices (GW x ED)
    for(uint s = 7; s <= 12; s++){
        
        // Build cluster matrix for this SF
        for(uint g = 0; g < gwCount; g++)
            clusters[s-7].push_back(l->getAllEDList(g ,s));
        
        // Check if SF has coverage
        bool hasCoverage = true; // This is updated if an ED cannot be assigned to any GW
        for(uint e = 0; e < edCount; e++){
            // If ed is not in essential list
            auto it = std::find(essED.begin(), essED.end(),e);
            if(it == essED.end()) { // If not found (not allocated)
                bool hasGW = false;
                for(uint g = 0; g < gwCount; g++){
                    // Find if ED "e" is present in the "clusters" tensor
                    auto it2 = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                    hasGW = (it2 != clusters[s-7][g].end());
                    if(hasGW) break; // Next ED
                }
                if(!hasGW){ 
                    hasCoverage = false;
                    std::cout << "No coverage for SF " << s << ": ED " << e << " cannot be assigned to any GW." << std::endl;
                    if(s == 12){ 
                        std::cout << "Exiting program." << std::endl;
                        exit(1);
                    }
                    break;
                }
            }
        }

        if(!hasCoverage){ 
            continue; // Next SF
        }else{ // Has coverage --> make allocation and eval objective function
            
            // Initialize GW List
            std::vector<uint>gwList;
            for(uint g = 0; g < gwCount; g++) {
                auto it = std::find(essGW.begin(), essGW.end(), g);
                if(it == essGW.end()) // If not essential, add to gw allocation list
                    gwList.push_back(g);
            }

            std::random_device rd;
            std::mt19937 gen(rd());

            for(uint iter = 0; iter < iters; iter++){ // Try many times                
                
                std::shuffle(gwList.begin(), gwList.end(), gen); // Shuffle list of gw

                // Start allocation of EDs one by one
                uint gw[edCount];
                uint sf[edCount];
                bool allAllocable = true;
                std::vector<UtilizationFactor> gwuf(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    uint allocGW, allocSF;
                    bool allocated = false;
                    
                    // Check if e already allocated (essential)
                    auto it = std::find(essED.begin(), essED.end(),e);
                    if(it == essED.end()) { // If not allocated to essential GW, allocate to available GW
                        for(uint gi = 0; gi < gwList.size(); gi++){
                            const uint g = gwList[gi];
                            // Check if ED e can be allocated to GW g
                            auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                            if((it != clusters[s-7][g].end()) && !gwuf[g].isFull()){
                                allocGW = g;
                                allocSF = l->getMinSF(e, g);
                                allocated = true; // Mark as valid allocation values
                                break; // Stop searching gw
                            }
                        }
                    }else{ // If allocated to essential GW, copy values
                        allocGW = gwBest[e];
                        allocSF = l->getMinSF(e, allocGW);
                        allocated = true; // Mark as valid allocation values
                    }
                    if(allocated){ // If valid allocation values
                        gw[e] = allocGW;
                        sf[e] = allocSF;
                        gwuf[allocGW] += l->getUF(e, allocSF);
                    }else{ // If it was not possible to allocate ED "e"
                        if(iter == iters){
                            std::cout << "Not possible to allocate ED " << e << ", trying with next iteration." << std::endl;
                            allAllocable = false;
                            break; // Go to next iteration
                        }
                    }
                } // Allocation finished

                if(allAllocable){ // Eval solution
                    uint gwUsed, energy; double uf; bool feasible;
                    const double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
                    if(feasible && cost < minimumCost){ // New optimum
                        minimumCost = cost;
                        std::copy(gw, gw + edCount, gwBest);
                        std::copy(sf, sf + edCount, sfBest);
                        feasibleFound = true;
                        
                        // Print status
                        std::cout << "New best at iteration: " << iter << " (SF = " << s << ")" << std::endl;
                        o->printSolution(gw, sf, false);
                        std::cout << std::endl << std::endl;
                    }
                }

                // Check if out of time
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
                if (elapsedSeconds >= timeout) {
                    std::cout << "Time limit reached." << std::endl;
                    timedout = true;
                    break;
                }
            }
        }
        if(timedout) break;   
    }

    OptimizationResults results;
    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    
    std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
    if(feasibleFound){
        std::cout << "Best result:" << std::endl;
        o->printSolution(gwBest, sfBest, true, true, true);
    }else{
        std::cout << "No feasible solution was found." << std::endl;
    }

    return results;
}
