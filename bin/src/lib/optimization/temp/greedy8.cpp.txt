#include "greedy.h"


OptimizationResults greedy8(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 8 -------------" << std::endl << std::endl;

    const uint gwCount = l->gwCount;
    const uint edCount = l->edCount;

    bool feasibleFound = false;    
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    uint gw[edCount];
    uint sf[edCount];
    
    if(verbose) std::cout << "Step 1: G4 allocation" << std::endl << std::endl;

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
                bool allEDAllocated = true;
                for(uint e = 0; e < edCount; e++){ 
                    bool edAllocated = false;
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = gwList[gi];
                        // Check if ED e can be connected to GW g
                        auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                        if((it != clusters[s-7][g].end()) && !gwuf[g].isFull()){
                            uint minsf = l->getMinSF(e, g);
                            gw[e] = g;
                            sf[e] = minsf; // Always assign lower SF
                            gwuf[g] += l->getUF(e, minsf);
                            edAllocated = true;
                            break; // Go to next ed
                        }
                    }
                    if(!edAllocated){
                        allEDAllocated = false;
                        break;
                    }
                } // Allocation finished
                if(!allEDAllocated) continue;

                // Eval solution
                uint gwUsed, energy; double uf; bool feasible;
                const double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
                if(feasible && cost < minimumCost){ // New optimum
                    minimumCost = cost;
                    std::copy(gw, gw + edCount, gwBest);
                    std::copy(sf, sf + edCount, sfBest);
                    feasibleFound = true;
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

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
        std::cout << "First step: (G4) finished in " << results.execTime << " ms" << std::endl;
        if(feasibleFound){
            std::cout << "Best:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true, false);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
            return results;
        }
    }

    

    if(verbose) std::cout << std::endl << "Step 2: reallocation" << std::endl << std::endl;
    
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

    // Start from first GW and try to reallocate all of its EDs to any of the following
    for (uint g = 0; g < results.gwUsed; g++) {
        const uint gIndex = sortedGWList[g];
        uint e = 0;
        while(e < sortedGWEDs[g].size()) { // For each ED of GW gIndex
            uint edIndex = sortedGWEDs[g][e]; // Number of ED
            std::vector<uint> availablesGWs = l->getSortedGWList(edIndex); // List of GW in range of this ED
            if (availablesGWs.size() > 1){
                for(uint g2 = 0; g2 < results.gwUsed; g2++) { // Search if possible to allocate to another GW puse el principio de la lista... 
                    const uint g2Index = sortedGWList[g2];
                    if(gIndex != g2Index){ 
                        auto it = std::find(availablesGWs.begin(), availablesGWs.end(), g2Index);
                        if(it != availablesGWs.end()){ // g2 is in available list, check if enough UF
                            const uint s = l->getMinSF(edIndex, g2Index); // Use minSF
                            UtilizationFactor uf = l->getUF(edIndex, s); // UF for this ED using the selected SF
                            if(!(sortedgwuf[g2] + uf).isFull()) { // This ED can be moved to g2
                                if (sfBest[edIndex] > s || sortedGWEDs[g].size() == 1){ // If lower SF or remove GW
                                    if(verbose) std::cout << "Reallocated ED " << edIndex << ": GW " << gIndex << " --> " << g2Index << ", SF: "<<sfBest2[edIndex]<<" --> " << s <<std::endl;	
                                    gwBest2[edIndex] = g2Index; // Reallocate ED to another GW
                                    sfBest2[edIndex] = s; // Reallocate ED to new SF
                                    sortedgwuf[g] -= uf; // Reduce UF of original GW (g)
                                    sortedGWEDs[g].erase(std::remove(sortedGWEDs[g].begin(), sortedGWEDs[g].end(), edIndex), sortedGWEDs[g].end()); // Remove ED e from GW g
                                    // Sort again sortedGWList
                                    std::sort(
                                        indirection.begin(), 
                                        indirection.end(), 
                                        [&sortedGWEDs](const uint &a, const uint &b) {
                                            return sortedGWEDs[a].size() < sortedGWEDs[b].size();
                                        }
                                    );
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            e++;
        }
    }
    
    OptimizationResults results2;
    results2.cost = o->eval(gwBest2, sfBest2, results.gwUsed, results.energy, results.uf, results.feasible);

    if(results2.cost < results.cost){
        std::cout << std::endl << "New optimum: " << results2.cost << " (previous: " << results.cost << ")" << std::endl << std::endl;
        std::copy(gwBest2, gwBest2 + edCount, gwBest);
        std::copy(sfBest2, sfBest2 + edCount, sfBest);
        results = results2;
    }else
        std::cout << "No improvement step 2: Cost " << results2.cost << std::endl << std::endl;

    //////////// Export results ////////////
    if(wst) o->exportWST(gwBest, sfBest);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true;
    if(verbose){
        std::cout << "Exec. time " << results.execTime << " ms" << std::endl;
        std::cout << "Best:" << std::endl;
        o->printSolution(gwBest, sfBest, true, true, true);
    }
    return results;
}