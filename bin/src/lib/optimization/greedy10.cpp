#include "greedy.h"


OptimizationResults greedy10(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 10 -------------" << std::endl << std::endl;

    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    uint gw[edCount];
    uint sf[edCount];
    
    std::vector<std::vector<std::vector<uint>>> clusters; // Clusters tensor (SF x GW x ED)
    clusters.resize(6); // Initialize list of matrices (GW x ED)
    for(uint s = 7; s <= 12; s++){
        
        // Build cluster matrix for this SF
        for(uint g = 0; g < gwCount; g++)
            clusters[s-7].push_back(l->getAllEDList(g ,s));
        
        // Check if SF has coverage
        bool hasCoverage = true; // This is updated if an ED cannot be assigned to any GW
        for(uint e = 0; e < edCount; e++){
            bool hasGW = false;
            for(uint g = 0; g < gwCount; g++){
                auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                hasGW = (it != clusters[s-7][g].end());
                if(hasGW) break;
            }
            if(!hasGW){ 
                hasCoverage = false;
                if(verbose) std::cout << "No coverage for SF " << s << ": ED " << e << " cannot be assigned to any GW." << std::endl;
                if(s == 12) exit(1);
                break;
            }
        }

        if(!hasCoverage) continue; // Next SF
        else{ // Has coverage --> make allocation and eval objective function
            
            // Initialize GW List
            std::vector<uint>gwList(gwCount);
            for(uint i = 0; i < gwCount; i++) gwList[i] = i;

            std::random_device rd;
            std::mt19937 gen(rd());

            for(uint iter = 0; iter < iters; iter++){ // Try many times 
                
                std::shuffle(gwList.begin(), gwList.end(), gen); // Shuffle list of gw
                
                // Start allocation of EDs one by one
                std::vector<UtilizationFactor> gwuf(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = gwList[gi];
                        // Check if ED e can be allocated to GW g
                        auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                        if((it != clusters[s-7][g].end()) && !gwuf[g].isFull()){
                            uint minsf = l->getMinSF(e, g);
                            gw[e] = g;
                            sf[e] = minsf; // Always assign lower SF
                            gwuf[g] += l->getUF(e, minsf);
                            break; // Go to next ed
                        }
                    }
                } // Allocation finished


                // Eval to get gwUsed
                uint gwUsed, energy; double uf; bool feasible;
                double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
                if(feasible && cost < minimumCost){ // New optimum
                    minimumCost = cost;
                    std::copy(gw, gw + edCount, gwBest); // Copy from gw to gwBest
                    std::copy(sf, sf + edCount, sfBest);
                    if(verbose){
                        std::cout << "New best at iteration: " << iter << " (SF = " << s << ")" << std::endl;
                        o->printSolution(gw, sf, false);
                        std::cout << std::endl;
                    }
                }

                // Check if out of time
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
                if (elapsedSeconds >= timeout) {
                    if(verbose) std::cout << "Time limit reached." << std::endl;
                    timedout = true;
                    break;
                }
            }
        }
        if(timedout) break;   
    }

    // G4 Allocation finished
    if(verbose) std::cout << std::endl << "Step 2: reallocation" << std::endl << std::endl;
    OptimizationResults results;
    results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
    
    uint gwBest2[edCount];
    uint sfBest2[edCount];
    std::copy(gwBest, gwBest + edCount, gwBest2);
    std::copy(sfBest, sfBest + edCount, sfBest2);
    
    std::vector<uint> gwList;
    std::vector<std::vector<uint>> gwEDs(results.gwUsed);
    std::vector<UtilizationFactor> gwuf; 
    std::vector<uint>indirection(results.gwUsed);
    std::iota(indirection.begin(), indirection.end(), 0);
    for(uint i = 0; i < edCount; i++){
        UtilizationFactor uf = l->getUF(i, sfBest[i]); // UF of ED i
        auto it = std::find(gwList.begin(), gwList.end(), gwBest[i]); // Find gw of ED i in list
        if(it != gwList.end()) { // If found, increase ED and UF
            uint index = std::distance(gwList.begin(), it);
            gwEDs[index].push_back(i); // Add ED to GW
            gwuf[index] += uf;
        }else{ // If not, add
            gwList.push_back(gwBest[i]); // Add gw index to list
            gwEDs[gwList.size()-1].push_back(i); // Add first ED to gw
            gwuf.push_back(uf); // Add initial UF 
        }
    }

    // Sort indirection array in ascending order of number of EDs
    std::sort(
        indirection.begin(), 
        indirection.end(), 
        [&gwEDs](const uint &a, const uint &b) {
            return gwEDs[a].size() < gwEDs[b].size();
        }
    );
    
    // Sort all arrays with indirection array
    std::vector<uint> sortedGWList(results.gwUsed);
    std::vector<std::vector<uint>> sortedGWEDs(results.gwUsed);
    std::vector<UtilizationFactor> sortedgwuf(results.gwUsed);
    for (uint i = 0; i < results.gwUsed; i++) {
        sortedGWList[i] = gwList[indirection[i]];
        sortedgwuf[i] = gwuf[indirection[i]];
        sortedGWEDs[i] = gwEDs[indirection[i]];
    }

    if(verbose) // Print ED count and UF for each Gw
        for (uint i = 0; i < results.gwUsed; i++) {
            std::cout << "GW " << sortedGWList[i] << ": " << sortedGWEDs[i].size() << " EDs." << std::endl;
            for(int s = 7; s <= 12; s++)
                std::cout << "  UF SF" << s << " = " << sortedgwuf[i].getUFValue(s) << std::endl;
            std::cout << std::endl;
        }

    


    /// Export results
    results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
    if(wst) o->exportWST(gwBest, sfBest);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true;
    if(verbose){
        std::cout << std::endl << "Exec. time " << results.execTime << " ms" << std::endl;
        std::cout << "Best:" << std::endl;
        o->printSolution(gwBest, sfBest, true, true, true);
    }
    return results;
}
