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
            bool condition = (minimize==MIN::GW && tempEDList.size() > edsOfSelectedGW.size()) ||
                            (minimize==MIN::E && minener < gwMinEnergy) ||
                            (minimize==MIN::UF && minUF < gwMinUF);
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

    if(wst) o->exportWST(gw, sf);

    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults results;
    results.cost = o->eval(gw, sf, results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
        std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
        std::cout << "Result:" << std::endl;
        o->printSolution(gw, sf, true, true);
    }

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

OptimizationResults greedy3(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 3 minimization -------------" << std::endl << std::endl;
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

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

OptimizationResults greedy5(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 5 minimization -------------" << std::endl << std::endl;

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

            std::vector<uint> permGWList(gwList.begin(), gwList.end()); // Permutations
            uint perm = 0; // Permutation index
            do {
                perm++;

                uint gw[edCount];
                uint sf[edCount];
                
                // Start allocation of EDs one by one
                std::vector<UtilizationFactor> gwuf(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = permGWList[gi];
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
                        std::cout << "New best at permGWList: " << perm << " (SF = " << s << ")" << std::endl;
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
            } while (std::next_permutation(permGWList.begin(), permGWList.end()) && !timedout);
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
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    return results;
}

OptimizationResults greedy6(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 6 minimization -------------" << std::endl << std::endl;

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

            if(o->tp.gamma >= 1){
                if(verbose) std::cout << "Using descending order of GW" << std::endl;
                std::sort(gwList.begin(), gwList.end(), [&clusters, &s](int a, int b) {
                    return clusters[s-7][a].size() > clusters[s-7][b].size(); // Descending order
                });
            }else{
                if(verbose) std::cout << "Using ascending order of GW" << std::endl;
                std::sort(gwList.begin(), gwList.end(), [&clusters, &s](int a, int b) {
                    return clusters[s-7][a].size() < clusters[s-7][b].size(); // Ascending order
                });
            }

            /* Check if correctly sorted
            for(uint gi = 0; gi < gwCount; gi++){
                std::cout << gwList[gi] << "(" << clusters[s-7][gwList[gi]].size() << ") ";
            }
            std::cout << std::endl;
            */    

            // Optimization variable
            uint gw[edCount];
            uint sf[edCount];
            
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
                    std::cout << "New best for SF = " << s << std::endl;
                    o->printSolution(gw, sf, false);
                    std::cout << std::endl << std::endl;
                }
            }else{
                std::cout << "Not optimum for SF = " << s << std::endl;
            }

            // Check if out of time
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
            if (elapsedSeconds >= timeout) {
                if(verbose) std::cout << "Time limit reached." << std::endl;
                break;
            }
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

OptimizationResults greedy7(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();

    if(verbose) std::cout << "------------- Greedy 7 minimization -------------" << std::endl << std::endl;

    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    bool feasibleFound = false;
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    bool timedout = false;

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

            if(o->tp.gamma >= 1){
                if(verbose) std::cout << "Using descending order of GW" << std::endl;
                std::sort(gwList.begin(), gwList.end(), [&clusters, &s](int a, int b) {
                    return clusters[s-7][a].size() > clusters[s-7][b].size(); // Descending order
                });
            }else{
                if(verbose) std::cout << "Using ascending order of GW" << std::endl;
                std::sort(gwList.begin(), gwList.end(), [&clusters, &s](int a, int b) {
                    return clusters[s-7][a].size() < clusters[s-7][b].size(); // Ascending order
                });
            }

            /* Check if correctly sorted
            for(uint gi = 0; gi < gwCount; gi++){
                std::cout << gwList[gi] << "(" << clusters[s-7][gwList[gi]].size() << ") ";
            }
            std::cout << std::endl;
            */    

            std::random_device rd;
            std::mt19937 gen(rd());

            for(uint iter = 0; iter < iters; iter++){ // Try many times                
                
                if(iter != 0) // shuffle list of GW
                    std::shuffle(gwList.begin(), gwList.end(), gen); // Shuffle list of gw
                

                // Optimization variable
                uint gw[edCount];
                uint sf[edCount];
                
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
                }// Allocation finished

                // Remove unused GWs from list
                uint count = 0;
                for(uint g = 0; g < gwCount; g++){
                    auto it = std::find(gwList.begin(), gwList.end(), g);
                    if(it != gwList.end() && !gwuf[g].isUsed()){
                        gwList.erase(it);
                        count++;
                    }
                }
                if(verbose) std::cout << "Removed " << count << " gateways from list." << std::endl;

                // Eval solution
                uint gwUsed, energy; double uf; bool feasible;
                const double cost = o->eval(gw, sf, gwUsed, energy, uf, feasible);
                if(feasible && cost < minimumCost){ // New optimum
                    minimumCost = cost;
                    std::copy(gw, gw + edCount, gwBest);
                    std::copy(sf, sf + edCount, sfBest);
                    feasibleFound = true;
                    if(verbose){
                        std::cout << "New best for SF = " << s << std::endl;
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
           if(timedout) break;
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


OptimizationResults greedy8(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose) std::cout << "------------- Greedy 8 minimization -------------" << std::endl << std::endl;

    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();

    bool feasibleFound = false;    
    uint gwBest[edCount];
    uint sfBest[edCount];
    double minimumCost = __DBL_MAX__;
    uint gw[edCount];
    uint sf[edCount];
    
    uint gwBestBest[edCount];
    uint sfBestBest[edCount];

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

                //uint gw[edCount];
                //uint sf[edCount];
                
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
            std::cout << "Best result so far:" << std::endl;
            o->printSolution(gwBest, sfBest, true, false, false);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
            return results;
        }
    }


    //////////// Reduce number of GW by reallocation of EDs ////////////

    double vueltaBest = __DBL_MAX__;
    for (uint vuelta=0;vuelta<1;vuelta++){
        
        std::cout << std::endl << "Started reallocation step" << std::endl << std::endl;
        // Create list of used gws, eds and and UFs
        std::vector<uint> gwList;
        std::vector<std::vector<uint>> gwEDs(results.gwUsed);
        std::vector<UtilizationFactor> gwuf; 
        // Indirection array to sort
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
        // Print ED count and UF for each Gw
        for (uint i = 0; i < results.gwUsed; i++) {
            std::cout << "GW " << sortedGWList[i] << " has " << sortedGWEDs[i].size() << " ED and the UFs are:" << std::endl;
            for(int s = 7; s <= 12; s++)
                std::cout << "   SF" << s << " = " << sortedgwuf[i].getUFValue(s) << std::endl;
            std::cout << std::endl;
        }
        // Start from first GW and try to reallocate all of its EDs to any of the following
        uint intentos = 0;
        for (uint g = 0; g < results.gwUsed; g++) { // For each GW
            const uint gIndex = sortedGWList[g];
            uint e = 0;
            uint z=0;
            uint matches=0;
            uint todos=0;
            uint ELIMINA=0;
            while(e < sortedGWEDs[g].size()) { // For each ED of GW gIndex
                uint edIndex = sortedGWEDs[g][e]; // Number of ED
                std::vector<uint> availablesGWs = l->getSortedGWList(edIndex); // List of GW in range of this ED
                std::cout<<"Lista de gateways posibles para nodo "<<edIndex<<": ";
                for (uint y=0;y<availablesGWs.size();y++){
                    std::cout<<availablesGWs[y]<<" ";
                }
                std::cout<<std::endl;
                //Busco si los nodos solamente tienen al gw indicado como posible.
                matches=0;
                std::cout<<"Analisis gw "<<gIndex<<std::endl;
                for (uint w=0;w<availablesGWs.size();w++){
                        for (uint v=0;v<sortedGWList.size();v++){
                            if (sortedGWList[v]==availablesGWs[w]){
                                matches++;
                            std::cout<<availablesGWs[w]<<" ";
                            }                        
                        }
                }
                    if (matches>1){
                        //std::cout<<"Nodo "<<edIndex<<" puede moverse"<<std::endl;
                    todos++;
                    }

                std::cout<<std::endl;
            e++;
            }
            if (todos==sortedGWEDs[g].size()){
                    std::cout<<"Eliminar gw "<<gIndex<<" todos "<<todos<<std::endl;
                    ELIMINA = 1;	
            }
            else{
                    std::cout<<"No eliminar gw "<<gIndex<<" todos "<<todos<<std::endl;            
                    ELIMINA = 0;
            }
            todos = 0;
            //e=0;
            
            z=0;
            while(z < sortedGWEDs[g].size()) { // For each ED of GW gIndex
                uint edIndex = sortedGWEDs[g][z]; // Number of ED
                std::vector<uint> availablesGWs = l->getSortedGWList(edIndex); // List of GW in range of this ED
        /* 	std::cout<<"Lista de gateways posibles para nodo "<<edIndex<<": ";
                for (uint y=0;y<availablesGWs.size();y++){
                    std::cout<<availablesGWs[y]<<" ";
                }
                std::cout<<std::endl;
                //Busco si los nodos solamente tienen al gw indicado como posible.
        //       matches=0;
                for (uint w=0;w<availablesGWs.size();w++){
                        for (uint v=0;v<sortedGWList.size();v++){
                            if (sortedGWList[v]==availablesGWs[w]){
                                matches++;
                            }                        
                        }
                        if (matches>1){
                            //std::cout<<"Nodo "<<edIndex<<" puede moverse"<<std::endl;
                        todos++;
                        }
                }
                if (todos==sortedGWEDs[g].size()){
                    std::cout<<"Eliminar gw"<<std::endl;
                }*/        
            //	std::cout<<"Size de availablesGWS "<<matches<<std::endl;                            
                std::cout<<"Chequea nodo: "<<edIndex<<" z "<<z<<std::endl;
                if (availablesGWs.size()>1){//era availablesGWs.size()>1x
                    std::cout<<"Analiza nodo "<<edIndex<<" en gw: "<<sortedGWList[g]<<std::endl;
                    for(uint g2 = 0; g2 < results.gwUsed; g2++) { // Search if possible to allocate to another GW puse el principio de la lista... 
                        const uint g2Index = sortedGWList[g2];
                        if(gIndex != g2Index){ 
                            auto it = std::find(availablesGWs.begin(), availablesGWs.end(), g2Index);
                            if(it != availablesGWs.end()){ // g2 is in available list, check if enough UF
                                const uint s = l->getMinSF(edIndex, g2Index); // Use minSF
                                UtilizationFactor uf = l->getUF(edIndex, s); // UF for this ED using the selected SF
                                if(!(sortedgwuf[g2] + uf).isFull()) { // This ED can be moved to g2
                                    if (ELIMINA==1){
                                    std::cout << "Moved ED " << edIndex << " from GW " << gIndex << " to GW " << g2Index << " with SF: "<<sfBest[edIndex]<<" to "<<s<<std::endl;	
                                    gwBest[edIndex] = g2Index; // Reallocate ED to another GW
                                    sfBest[edIndex] = s; // Reallocate ED to new SF
                                    sortedgwuf[g] -= uf; // Reduce UF of original GW (g)
                                    sortedGWEDs[g].erase(std::remove(sortedGWEDs[g].begin(), sortedGWEDs[g].end(), edIndex), sortedGWEDs[g].end()); // Remove ED e from GW g
                                    if (sortedGWEDs[g].size()>0)
                                        z--;
                                    break; // Stop searching for another g2
                                }
                                    else{
                                    if (sfBest[edIndex]>s){
                                        std::cout << "Moved ED " << edIndex << " from GW " << gIndex << " to GW " << g2Index << " with SF: "<<sfBest[edIndex]<<" to "<<s<<std::endl;	
                                        gwBest[edIndex] = g2Index; // Reallocate ED to another GW
                                        sfBest[edIndex] = s; // Reallocate ED to new SF
                                        sortedgwuf[g] -= uf; // Reduce UF of original GW (g)
                                        sortedGWEDs[g].erase(std::remove(sortedGWEDs[g].begin(), sortedGWEDs[g].end(), edIndex), sortedGWEDs[g].end()); // Remove ED e from GW g
                                        if (sortedGWEDs[g].size()>0)
                                                z--;
                                        break; // Stop searching for another g2
                                    }
                                    else{
                                        sortedGWEDs[g].erase(std::remove(sortedGWEDs[g].begin(), sortedGWEDs[g].end(), edIndex), sortedGWEDs[g].end()); // Remove ED e from GW g
                                        break; // Stop searching for another g2                          
                                    }
                                }
                                }
                            }
                        }
                    }
                }
    //     		else{
    //      			break;
    //     	}
            z++; // Next ED iW       
            }
            if (ELIMINA==1){
                //std::cout << "Eliminando: " << g << " Intentos: " << intentos << std::endl;
                sortedGWList.erase(std::remove(sortedGWList.begin(), sortedGWList.end(), g), sortedGWList.end());
                g--;
                intentos++;
                if(intentos > 1000){
                    break;
                }
            }
        }
        
        OptimizationResults results2;
        results2.cost = feasibleFound ? o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
        if(results2.cost < vueltaBest){
            std::cout << "Nuevo optimo valor: " << results2.cost << std::endl;
            std::copy(gwBest, gwBest + edCount, gwBestBest);
            std::copy(sfBest, sfBest + edCount, sfBestBest);
        }
    }

    //////////// Show final results ////////////

    if(wst) o->exportWST(gwBestBest, sfBestBest);

    results.cost = feasibleFound ? o->eval(gwBestBest, sfBestBest, results.gwUsed, results.energy, results.uf, results.feasible) : __DBL_MAX__;
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
        std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
        if(feasibleFound){
            std::cout << "Best result:" << std::endl;
            o->printSolution(gwBestBest, sfBestBest, true, true, true);
        }else{
            std::cout << "No feasible solution was found." << std::endl;
        }
    }

    return results;
}
