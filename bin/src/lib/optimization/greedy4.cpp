#include "greedy.h"


OptimizationResults greedy4(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 4 minimization -------------" << std::endl << std::endl;

    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    bool feasibleFound = false;    
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;

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
                /* Find if ED "e" is present in the "clusters" tensor
                for(uint ee = 0; ee < clusters[s-7][g].size(); ee++){
                    if(clusters[s-7][g][ee] == e){
                        hasGW = true;
                        break;
                    }
                }
                */
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

                uint gw[edCount];
                uint sf[edCount];
                
                // Start allocation of EDs one by one
                bool allEDAllocated = true;
                std::vector<UtilizationFactor> gwuf(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    bool edAllocated = false;
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = gwList[gi];
                        // Check if ED e can be allocated to GW g
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
                }// Allocation finished
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
                        std::cout << std::endl << std::endl;
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
            o->printSolution(gwBest, sfBest, true, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    return results;
}