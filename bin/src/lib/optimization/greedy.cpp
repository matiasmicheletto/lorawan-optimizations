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

        // Repeat until no more EDs can be elimina
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
