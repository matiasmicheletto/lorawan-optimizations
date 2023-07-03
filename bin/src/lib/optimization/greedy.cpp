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

    // Use a copy of the raw data
    std::vector<std::vector<uint>> matrix;
    l->copySFDataTo(matrix);

    uint loopCounter = 0;
    while(matrix.size() > 0) {
        // Get number of ED that can be associated to each GW
        // Find index (column or gw number) with maximum value
        uint maxED = 0; // Max number of ed
        uint gwMaxED = 0;
        for(uint j = 0; j < l->getGWCount(); j++) {
            std::vector<uint> edList = l->getEDList(j);
            if(edList.size() > maxED){
                maxED = edList.size();
                gwMaxED = j;
            }
        } 

        // Get list of all EDs indexes associated with current GW
        std::vector<uint> edList = l->getEDList(gwMaxED);

        // Set values in GW and SF arrays 
        // TODO

        // Remove column of max GW and all its EDs from copy matrix
        removeRowsAndColumn(matrix, gwMaxED, edList);

        // Repeat until matrix is empty or no more rows can be removed
        loopCounter++;
        if(loopCounter > l->getGWCount()) {
            std::cerr << "Error: Unfeasible system. An End-Device cannot be allocated to any Gateway." << std::endl;
            exit(1);
        }
    }

    // Evaluate GW and SF pair of vectors with objective function and return results
    OptimizationResults opr;
    char solverName[7] = "Greedy";
    opr.cost = o->eval(gw, sf, opr.gwUsed, opr.energy, opr.uf);
    opr.tp = o->tp;
    opr.solver = solverName;
    opr.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    opr.withResults = true; // Set export flag to ready

    // Release memory used for optimization variable
    free(gw);
    free(sf);

    return opr;
}