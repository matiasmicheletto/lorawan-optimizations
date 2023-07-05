#include "greedy.h"

OptimizationResults greedy(Instance* l, Objective* o, uint maxIters, bool verbose){
    auto start = std::chrono::high_resolution_clock::now();
    
    if(verbose)
        std::cout << std::endl << "------------- Greedy -------------" << std::endl << std::endl;

    // Optimization variable
    uint* gw = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sf = (uint*) malloc( sizeof(uint) * l->getEDCount());
    for(uint i = 0; i < l->getEDCount(); i++){
        gw[i] = 0;
        sf[i] = 0;
    }
    
    std::vector<uint> eds(l->getEDCount());
    std::vector<uint> gws(l->getGWCount());
    std::iota(eds.begin(), eds.end(), 0); // Initialization: 0,1,2,3, ....
    std::iota(gws.begin(), gws.end(), 0); // Initialization: 0,1,2,3, ....

    uint loopCounter = 0; // Count number of times the loop is executed (to prevent infinite loop)
    while(!eds.empty()) { // While eds to be allocated
        // Get index of the GW that can be associated to a larger number of EDs
        uint gwMaxED = 0; // Index of GW with max (initially pointing to the first GW)
        std::vector<uint> maxEdList; // Larger list of EDs associated to a GW
        for(uint j = 0; j < gws.size(); j++) { // For each GW, get attribute (in this case, feasible EDs)
            double accumulatedUF = 0.0; // UF of current GW
            std::vector<uint> edList; // List of EDs for current GW (initially empty)
            for(uint i = 0; i < eds.size(); i++){ // For each unallocated ED, check if feasible to current GW
                uint edToTest = eds.at(i);
                const uint minSF = l->getMinSF(edToTest, j);
                const uint maxSF = l->getMaxSF(edToTest);
                double uf = l->getUF(edToTest, minSF); // TODO: using always min SF ?? or max SF ??
                if(minSF <= maxSF && (uf+accumulatedUF) < 1.0){
                    edList.push_back(edToTest);
                    accumulatedUF += uf; 
                }
            }
            if(edList.size() > maxEdList.size()){ // If greater than current, update
                maxEdList.resize(edList.size());
                std::copy(edList.begin(), edList.end(), maxEdList.begin());
                gwMaxED = gws.at(j); // Update index of GW with max EDs
            }
        } 

        // Remove EDs from list
        uint removedEDs = 0;
        for(uint i = 0; i < maxEdList.size(); i++){
            for(uint j = 0; j < eds.size(); j++){ // Search for ED index
                uint edToRemove = maxEdList.at(i);
                if(eds.at(j) == edToRemove){
                    gw[edToRemove] = gwMaxED;
                    sf[edToRemove] = l->getMinSF(edToRemove, gwMaxED); // TODO: same as before, use min or max??
                    eds.erase(eds.begin()+j);
                    removedEDs++;
                    break; // Next ed
                }
            }
        }

        // Remove GW with max number of EDs
        auto gwIdx = std::find(gws.begin(), gws.end(), gwMaxED);
        if (gwIdx != gws.end())
            gws.erase(gwIdx);  

        // Repeat until no more EDs can be removed
        loopCounter++; // Included to avoid infinite loop
        if(loopCounter > l->getGWCount()) {
            std::cerr << "Error: Unfeasible system. An End-Device cannot be allocated to any Gateway." << std::endl;
            exit(1);
        }
    }

    if(verbose){
        std::cout << "Result:" << std::endl;
        o->printSolution(gw, sf, true, true);
    }

    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults results;
    results.cost = o->eval(gw, sf, results.gwUsed, results.energy, results.uf);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    // Release memory used for optimization variable
    free(gw);
    free(sf);

    return results;
}
