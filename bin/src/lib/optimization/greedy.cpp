#include "greedy.h"

OptimizationResults greedy(Instance* l, Objective* o, MIN minimize, bool verbose, bool wst){
    auto start = std::chrono::high_resolution_clock::now();
    
    if(verbose){
        std::cout << std::endl;
        switch (minimize)
        {
        case MIN::GW:
            std::cout << "------------- Greedy GW minimization -------------";
            break;
        case MIN::E:
            std::cout << "------------- Greedy Energy minimization -------------";
            break;
        case MIN::UF:
            std::cout << "------------- Greedy UF minimization -------------";
            break;
        default:
            std::cerr << "Error: Invalid greedy algorithm" << std::endl;
            exit(1);
            break;
        }
        std::cout << std::endl << std::endl;
    }

    // Allocate optimization variable (initialization should not be required)
    uint* gw = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sf = (uint*) malloc( sizeof(uint) * l->getEDCount());
    
    // Initialize stacks of GWs and EDs indexes
    std::vector<uint> edStack(l->getEDCount());
    std::vector<uint> gwStack(l->getGWCount());
    std::iota(edStack.begin(), edStack.end(), 0); // Initialization: 0,1,2,3, ....
    std::iota(gwStack.begin(), gwStack.end(), 0); // Initialization: 0,1,2,3, ....

    uint loopCounter = 0; // Count number of times the loop is executed (to prevent infinite loop)
    while(!edStack.empty()) { // While edStack to be allocated
        // Get index of the GW that can be associated to a larger number of EDs
        uint selectedGW = 0; // Index of GW with max (initially pointing to the first GW)
        uint gwMinEnergy = __UINT32_MAX__; // Min energy computed until now
        double gwMinUF = __DBL_MAX__; // Min UF computed until now
        std::vector<uint> edsOfSelectedGW; // Larger list of EDs associated to a GW
        for(uint j = 0; j < gwStack.size(); j++) { // For each available GW, get attribute (in this case, feasible EDs)
            uint gwToTest = gwStack[j]; // Count eds for each GW in stack
            UtilizationFactor gwToTestUF; // UF of GW to test
            std::vector<uint> tempEDList; // List of EDs for current GW to test (is initially empty)
            uint minener = 0; // Used for energy minimization
            double minUF = 0.0; // Used for UF minimization
            for(uint i = 0; i < edStack.size(); i++){ // For each unallocated ED, check if feasible to GW to test
                uint edToTest = edStack[i];
                // Get range of possible SF and compute potential UF
                const uint minSF = l->getMinSF(edToTest, gwToTest);
                const uint maxSF = l->getMaxSF(edToTest);
                UtilizationFactor edToTestUF = l->getUF(edToTest, minSF); // TODO: using always min SF ?? or max SF ??
                if(minSF <= maxSF && !((edToTestUF + gwToTestUF).isFull())){ // Feasibility condition for testint ED and GW
                    tempEDList.push_back(edToTest);
                    gwToTestUF += edToTestUF; 
                    minener += l->sf2e(minSF);
                    minUF += edToTestUF.getMax();
                }
            }
            bool condition = false;
            switch(minimize){ // Switch depending on component to minimize
                case MIN::GW: {
                    condition = tempEDList.size() > edsOfSelectedGW.size();
                    break;
                }
                case MIN::E: {
                    condition = minener < gwMinEnergy;
                    break;
                }
                case MIN::UF: {
                    condition = minUF < gwMinUF;
                    break;
                }
                default: 
                    std::cerr << "Error: Invalid greedy algorithm" << std::endl;
                    exit(1);
                    break;
            }
            if(condition) { // If a better allocation found, update
                edsOfSelectedGW.resize(tempEDList.size());
                std::copy(tempEDList.begin(), tempEDList.end(), edsOfSelectedGW.begin());
                selectedGW = gwStack[j]; // Update index of GW with min UF
                gwMinEnergy = minener;
                gwMinUF = minUF;
            }
        } 

        // Remove EDs from list
        for(uint i = 0; i < edsOfSelectedGW.size(); i++){
            uint edToRemove = edsOfSelectedGW[i];
            for(uint j = 0; j < edStack.size(); j++){ // Search for ED index
                if(edStack[j] == edToRemove){
                    gw[edToRemove] = selectedGW;
                    sf[edToRemove] = l->getMinSF(edToRemove, selectedGW); // TODO: same as before, use min or max??
                    edStack.erase(edStack.begin()+j);
                    break; // Next ed
                }
            }
        }

        // Remove selected GW from list
        auto gwToRemoveIndex = std::find(gwStack.begin(), gwStack.end(), selectedGW);
        if (gwToRemoveIndex != gwStack.end())
            gwStack.erase(gwToRemoveIndex);  

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

    if(wst) o->exportWST(gw, sf);

    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults results;
    results.cost = o->eval(gw, sf, results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    // Release memory used for optimization variable
    free(gw);
    free(sf);

    return results;
}

OptimizationResults greedy2(Instance* l, Objective* o, bool verbose, bool wst){
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 2 minimization -------------" << std::endl << std::endl;
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

    if(verbose) std::cout << std::endl << "Step 2: SF eval" << std::endl;

    // STEP 2 
    uint gw[edCount];
    uint sf[edCount];
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    bool feasibleFound = false;
    std::vector<uint>allocatedEDs;
    
    for(uint g = 0; g < gwCount; g++){ // Allocate end devices
        UtilizationFactor guf;
        //double guf = 0.0;
        for(uint s = 7; s <= 12; s++){ // For each spread factor        
            //double pw = (double) l->sf2e(s);   
            std::vector<uint> edList = l->getEDList(g, s);                
            for(uint e = 0; e < edList.size(); e++){
                uint selectedED = edList[e];
                if(std::find(allocatedEDs.begin(), allocatedEDs.end(), selectedED) == allocatedEDs.end()){ // If not allocated                    
                    UtilizationFactor u = l->getUF(selectedED, s);
                    //double u = pw / ((double)l->getPeriod(selectedED) - pw);
                    if(!(guf + u).isFull()){ // If allowed to allocate (enough uf)
                    //if(guf+u < 1.0){
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

    if(wst) o->exportWST(gwBest, sfBest);

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    return results;
}

OptimizationResults greedy3(Instance* l, Objective* o, unsigned long iters, bool verbose, bool wst){
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 3 minimization -------------" << std::endl << std::endl;
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    bool feasibleFound = false;    
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    
    if(verbose) std::cout << "Running " << iters << " iterations..." << std::endl << std::endl;

    for(unsigned long iter = 0; iter < iters; iter++){
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
    }

    if(verbose){
        if(feasibleFound){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    if(wst) o->exportWST(gwBest, sfBest);

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    return results;
}

OptimizationResults greedy4(Instance* l, Objective* o, unsigned long iters, bool verbose, bool wst){

    const int TIMEOUT = 5;  // 5 seconds
    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

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
            clusters[s-7].push_back(l->getEDList(g, s));
        
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
                break;
            }
        }

        if(!hasCoverage) continue; // Next SF
        else{ // Has coverage --> make allocation and eval objective function
            for(unsigned long iter = 0; iter < iters; iter++){ // Try many times
                // Shuffle list of gw
                uint gw[edCount];
                uint sf[edCount];
                std::vector<uint>gwList(gwCount);
                for(uint i = 0; i < gwCount; i++) gwList[i] = i;
                std::random_device rd;
                std::mt19937 gen(rd());
                std::shuffle(gwList.begin(), gwList.end(), gen);

                // Start allocation of EDs one by one
                for(uint e = 0; e < edCount; e++){ 
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = gwList[gi];

                        // Check if ED e can be allocated to GW g
                        auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                        if((it != clusters[s-7][g].end())){
                            gw[e] = g;
                            sf[e] = l->getMinSF(e, g); // Always assign lower SF
                            break; // Go to next ed
                        }
                    }
                }// Allocation finished

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
                if (elapsedSeconds >= TIMEOUT) {
                    if(verbose) std::cout << "Time limit reached." << std::endl;
                    timedout = true;
                    break;
                }
            }
        }
        if(timedout) break;   
    }

    if(verbose){
        if(feasibleFound){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    if(wst) o->exportWST(gwBest, sfBest);

    OptimizationResults results;

    results.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    return results;
}