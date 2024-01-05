#include "greedy.h"


OptimizationResults greedy(Instance* l, Objective* o, uint iters, uint timeout, bool verbose){

    if(verbose) std::cout << "------------- Greedy minimization -------------" << std::endl << std::endl;

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
    if(verbose) std::cout << std::endl << "Stage 1 -- Find essential nodes" << std::endl;
    std::vector<uint> essGW;
    std::vector<uint> essED;
    std::vector<UtilizationFactor> essGWUF; // Utilization factors of essential GWs
    for(uint g = 0; g < gwCount; g++){
        bool isEssential = false;
        uint allocatedCount = 0;
        const std::vector<uint> edList = l->getAllEDList(g, 12);
        //  First pass: Search for essential nodes and allocate to GW
        for(uint i = 0; i < edList.size(); i++){ 
            const uint e = edList[i];
            if(l->getGWList(e).size() == 1){ // GW g is essential for node e
                isEssential = true;
                essED.push_back(e);
                // Add gw to esential list (if not in list)
                auto it = std::find(essGW.begin(), essGW.end(), g);
                uint essGWIndex;
                if(it == essGW.end()){
                    essGW.push_back(g);
                    essGWUF.push_back(UtilizationFactor());
                    essGWIndex = essGW.size()-1;
                }else{
                    essGWIndex = std::distance(essGW.begin(), it);
                }
                // Try to allocate essential ed to essential gw
                uint tempSF = l->getMinSF(e, g);
                if(!(essGWUF[essGWIndex] + l->getUF(e, tempSF)).isFull()){
                    gwBest[e] = g;
                    sfBest[e] = tempSF;
                    essGWUF[essGWIndex] += l->getUF(e, tempSF);
                    allocatedCount++;
                }else{
                    std::cout << "ED " << e << " cannot be allocated to essential GW " << g << std::endl;
                    std::cout << "Unfeasible system. Exiting program..." << std::endl;
                    exit(1);
                }
            }
        }
        if(isEssential && verbose)
            std::cout << "Essential GW " << g << " has " << edList.size() << " reachable nodes, " << allocatedCount << " essential nodes allocated." << std::endl;
    }

    if(verbose){
        std::cout << std::endl << "Essential GWs: " << essGW.size() << " (of " << gwCount << ")" << std::endl;
        std::cout << "Total allocated nodes to essential GWs: " << essED.size() << std::endl;
    }

    for(uint e = 0; e < edCount; e++){ // Allocate non essential nodes to essential GWs
        auto it = std::find(essED.begin(), essED.end(), e);
        if(it == essED.end()){ // Remaining EDs (non essentials)
            const std::vector<uint> gwList = l->getSortedGWList(e);
            for(uint i = 0; i < gwList.size(); i++){ // Find essential GW in list of reachable
                const uint g = gwList[i];
                auto it2 = std::find(essGW.begin(), essGW.end(), g);
                if(it2 != essGW.end()){ // Is essential -> try to allocate to that gw
                    const uint essGWIndex = std::distance(essGW.begin(), it2);
                    uint tempSF = l->getMinSF(e, g);
                    if(!(essGWUF[essGWIndex] + l->getUF(e, tempSF)).isFull()){
                        gwBest[e] = g;
                        sfBest[e] = tempSF;
                        essGWUF[essGWIndex] += l->getUF(e, tempSF);
                        essED.push_back(e);
                        break; // Next ED
                    }
                }
            }
        }
    }

    if(verbose){
        std::cout << "Total non-essential nodes allocated to essential GWs: " << essED.size() << std::endl;
        std::cout << std::endl << "Stage 2 -- Allocation of non-essential nodes" << std::endl;
    }

    ////// G4 //////

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
                    if(verbose) std::cout << "No coverage for SF" << s << ": ED " << e << " cannot be assigned to any GW." << std::endl;
                    if(s == 12){ 
                        std::cout << "No coverage for SF 12. Unfeasible system." << std::endl;
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
            if(verbose) std::cout << "In SF" << s << " there are " << gwList.size() << " available GWs. Running iterations..." << std::endl;

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
                    auto it = std::find(essED.begin(), essED.end(), e);
                    if(it == essED.end()) { // If not allocated to essential GW, allocate to another available GW
                        for(uint gi = 0; gi < gwList.size(); gi++){
                            const uint g = gwList[gi];
                            // Check if ED e can be allocated to GW g
                            auto it2 = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                            uint tempSF = l->getMinSF(e, g);
                            if((it2 != clusters[s-7][g].end()) && !(gwuf[g] + l->getUF(e, tempSF)).isFull()){
                                allocGW = g;
                                allocSF = tempSF;
                                allocated = true; // Mark as valid allocation values
                                break; // Stop searching gw
                            }
                        }
                    }else{ // If allocated to essential GW, copy values of GW and SF
                        if(!(gwuf[gwBest[e]] + l->getUF(e, sfBest[e])).isFull()){
                            allocGW = gwBest[e];
                            allocSF = sfBest[e];
                            allocated = true; // Mark as valid allocation values
                        }else{
                            std::cout << "Iteration " << iter << ", trying to allocate ED " << e << " to GW " << gwBest[e] << " but UF>1. UF Values:" << std::endl;
                            gwuf[gwBest[e]].printUFValues(); 
                            std::cout << std::endl;
                        }
                    }
                    if(allocated){ // If valid allocation values
                        gw[e] = allocGW;
                        sf[e] = allocSF;
                        gwuf[allocGW] += l->getUF(e, allocSF);
                    }else{ // If it was not possible to allocate ED "e", jump to next iteration
                        allAllocable = false;
                        /*
                        std::cout << "ED " << e << " not allocable. GWs: ";
                        std::vector<uint> glist = l->getGWList(e);
                        for(uint i = 0; i < glist.size(); i++)
                            std::cout << glist[i] << " ";
                        std::cout << std::endl;
                        */
                        break; // Do not try next ED
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
                        if(verbose){ 
                            std::cout << std::endl << "New best at iteration: " << iter << " (SF = " << s << ")" << std::endl;
                            o->printSolution(gw, sf, false);
                        }
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
            if(verbose) std::cout << iters << " iterations completed." << std::endl;
        }
        if(timedout) break;   
    }

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    
    if(verbose){ 
        std::cout << std::endl << "Stage 2 finished in " << results.execTime << " ms" << std::endl;
        if(feasibleFound){
            std::cout << "Best:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true, false);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }
    if(!feasibleFound) return results;
    

    if(verbose) std::cout << std::endl << "Stage 3 -- Reallocation of ED" << std::endl;
    
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

    if(verbose)
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
        if(verbose) std::cout << std::endl << "New optimum: " << results2.cost << " (previous: " << results.cost << ")" << std::endl << std::endl;
        std::copy(gwBest2, gwBest2 + edCount, gwBest);
        std::copy(sfBest2, sfBest2 + edCount, sfBest);
        results = results2;
    }else
        if(verbose) std::cout << "No improvement in stage 3. Cost = " << results2.cost << std::endl << std::endl;

    //////////// Export results ////////////
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true;
    if(verbose) std::cout << "Total exec. time " << results.execTime << " ms" << std::endl;
    if(verbose) std::cout << "Best:" << std::endl;

    o->printSolution(gwBest, sfBest, true, true, true);

    return results;
}
