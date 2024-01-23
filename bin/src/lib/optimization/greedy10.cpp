#include "greedy.h"


OptimizationResults greedy10(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 10 -------------" << std::endl << std::endl;

    const uint gwCount = l->gwCount;
    const uint edCount = l->edCount;

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
            std::vector<uint>usedGWList(gwCount);
            for(uint i = 0; i < gwCount; i++) usedGWList[i] = i;

            std::random_device rd;
            std::mt19937 gen(rd());

            for(uint iter = 0; iter < iters; iter++){ // Try many times 
                
                std::shuffle(usedGWList.begin(), usedGWList.end(), gen); // Shuffle list of gw
                
                // Start allocation of EDs one by one
                std::vector<UtilizationFactor> ufOfUsedGWs(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = usedGWList[gi];
                        // Check if ED e can be allocated to GW g
                        auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                        if((it != clusters[s-7][g].end()) && !ufOfUsedGWs[g].isFull()){
                            uint minsf = l->getMinSF(e, g);
                            gw[e] = g;
                            sf[e] = minsf; // Always assign lower SF
                            ufOfUsedGWs[g] += l->getUF(e, minsf);
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
    


    for(uint iter = 0; iter < 5; iter++){
        uint gwUsed, energy;
        bool feasible;
        double uf;
        o->eval(gwBest, sfBest, gwUsed, energy, uf, feasible);
        
        uint gwBest2[edCount];
        uint sfBest2[edCount];
        std::copy(gwBest, gwBest + edCount, gwBest2);
        std::copy(sfBest, sfBest + edCount, sfBest2);
        
        std::vector<uint> usedGWList;
        std::vector<UtilizationFactor> ufOfUsedGWs;
        std::vector<std::vector<uint>> edOfUsedGWs(gwUsed);
        for(uint e = 0; e < edCount; e++){
            UtilizationFactor uf = l->getUF(e, sfBest[e]); // UF of ED i
            auto it = std::find(usedGWList.begin(), usedGWList.end(), gwBest[e]); // Find gw of ED i in list
            if(it != usedGWList.end()) { // If found, increase ED and UF
                uint index = std::distance(usedGWList.begin(), it);
                edOfUsedGWs[index].push_back(e); // Add ED to GW
                ufOfUsedGWs[index] += uf;
            }else{ // If not, add
                usedGWList.push_back(gwBest[e]); // Add gw index to list
                edOfUsedGWs[usedGWList.size()-1].push_back(e); // Add first ED to gw
                ufOfUsedGWs.push_back(uf); // Add initial UF 
            }
        }

        // Ordenar por indireccion lista de gws de mas ed a menos
        std::vector<uint>indirection(gwUsed);
        std::iota(indirection.begin(), indirection.end(), 0);
        std::sort(
            indirection.begin(), 
            indirection.end(), 
            [&edOfUsedGWs](const uint &a, const uint &b) {
                return edOfUsedGWs[a].size() < edOfUsedGWs[b].size();
            }
        );

        
        if(verbose){ // Imprimir asignacion por GW
            std::cout << "####### UF #########" << std::endl;
            for (uint i = 0; i < gwUsed; i++) {
                const uint g = indirection[i];
                std::cout << "GW " << usedGWList[g] << ": " << edOfUsedGWs[g].size() << " EDs." << std::endl;
                for(int s = 7; s <= 12; s++)
                    std::cout << "  UF SF" << s << " = " << ufOfUsedGWs[g].getUFValue(s) << std::endl;
                std::cout << std::endl;
            }

            std::cout << "####### Reallocation #########" << std::endl;
            for (uint i = 0; i < gwUsed; i++) {
                const uint gIndex = indirection[i];
                std::cout << "GW " << usedGWList[gIndex] << " (" << edOfUsedGWs[gIndex].size() << " EDs)" << std::endl;
                for(uint e = 0; e < edOfUsedGWs[gIndex].size(); e++){
                    const uint ed = edOfUsedGWs[gIndex][e];
                    std::vector<uint> listOfGWForED = l->getSortedGWListByAvailableEd(ed);
                    std::cout << "Available GW for ED " << ed << ": ";
                    for(uint g2 = 0; g2 < listOfGWForED.size(); g2++)
                        std::cout << listOfGWForED[g2] << " ";
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }

        for (uint i = 0; i < gwUsed; i++) { // Para cada gw usado, recorrer EDs para reasignar o agregar gw sin usar
            const uint gIndex = indirection[i];
            const uint g = usedGWList[gIndex];
            for(uint e = 0; e < edOfUsedGWs[gIndex].size(); e++){ // Para cada nodo asignado a este gw
                const uint ed = edOfUsedGWs[gIndex][e];
                std::vector<uint> listOfGWForED = l->getSortedGWListByAvailableEd(ed);
                for(uint j = 0; j < listOfGWForED.size(); j++){ // Para cada gw disponible para este ed
                    // Buscar si el gw ya fue usado (para reasignar)
                    const uint g2 = listOfGWForED[j];
                    bool found = false;
                    uint gfound;
                    for(uint jj = i+1; jj < gwUsed; jj++){
                        uint gx = usedGWList[indirection[jj]];
                        if(gx == g2){ 
                            found = true;
                            gfound = gx;
                            break;
                        }
                    }
                    if(found){ // Reasignar
                        const uint s = l->getMinSF(ed, gfound);
                        if(verbose) std::cout << "Reallocated ED " << ed << ": GW " << g << " --> " << gfound << ", SF: "<< sfBest2[ed] <<" --> " << s <<std::endl;	
                        gwBest2[ed] = gfound; 
                        sfBest2[ed] = s;
                        // Sacar ed de la lista y agregar al otro gw
                        auto it = std::find(edOfUsedGWs[gIndex].begin(), edOfUsedGWs[gIndex].end(), ed);
                        edOfUsedGWs[gIndex].erase(it);
                    }else{ // Si no fue usado, incorporar nuevo gw y recorrer la asignacion
                        std::cout << g2 << " not found, inserted to list." << std::endl;           
                        usedGWList.push_back(g2);
                        goto end;
                    }
                }
            }
            end: break;
        }
        std::cout << std::endl << "######## Iter " << iter << ", GW used = " << gwUsed << " ############" << std::endl;
    }



    /// Export results
    OptimizationResults results;
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
