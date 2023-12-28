#include "greedy.h"


OptimizationResults heuristica_rod(Instance* l, Objective* o, uint iters, uint timeout, bool verbose, bool wst){

    auto start = std::chrono::high_resolution_clock::now();
    bool feasibleFound = false;    
    const uint gwCount = l->getGWCount();
    const uint edCount = l->getEDCount();
    uint gwBest[edCount];
    uint sfBest[edCount];

    
    std::vector<std::vector<uint>> S(edCount, std::vector<uint>(gwCount+1));
    for (uint e = 0; e < edCount; e++){
        const uint maxSF = l->getMaxSF(e);
        for (uint g = 0; g < gwCount; g++){
            const uint minSF = l->getMinSF(e, g);
            S[e][g] = maxSF >= minSF ? minSF : 100;
        }
    }

    std::vector<uint> A(edCount, 0);
    std::vector<uint> G(gwCount, 0);
    for (uint e = 0; e < edCount; e++)
        A[e] = l->getGWList(e).size();
    for (uint g = 0; g < gwCount; g++)
        G[g] = l->getEDList(g, 12).size();

    std::vector<uint> E(gwCount, 0);
    for (uint e = 0; e < edCount; e++) 
        if (A[e] == 1)
            for (uint g = 0; g < gwCount; g++)
                if (S[e][g] < 100)
                    E[g] = 1;

    std::vector<std::vector<uint>> GW(gwCount, std::vector<uint>(edCount, 0));
    for (uint g = 0; g < gwCount; g++)
        if (E[g] == 1) 
            for (uint e = 0; e < edCount; e++)
                if (S[e][g] < 100) {
                    GW[g][e] = S[e][g];
                    for (uint k = 0; k < gwCount; k++)
                        S[e][k] = 100;
                }

    while (std::accumulate(E.begin(), E.end(), 0) < (int) edCount) {
        uint maxG = 0;
        uint longitud = 0;
        for (uint g = 0; g < gwCount; g++) {
            if (E[g] == 0) {
                uint count = std::count_if(S.begin(), S.end(), [g, edCount](const auto& row) {
                    return row[g] < edCount;
                });
                if (longitud < count) {
                    maxG = g;
                    longitud = count;
                }
            }
        }
        if (maxG == 0) break;
        E[maxG] = 1;
        for (uint e = 0; e < edCount; e++)
            if (S[e][maxG] < edCount) {
                GW[maxG][e] = S[e][maxG];
                for (uint k = 0; k < gwCount; k++)
                    S[e][k] = edCount;
            }
    }

    uint eng = 0;
    for (uint g = 0; g < gwCount; g++)
        for (uint e = 0; e < edCount; e++)
            if (GW[g][e] > 0)
                eng += std::pow(2, GW[g][e] - 7);

    uint CG = std::accumulate(E.begin(), E.end(), 0);    
    std::cout << "CG: " << CG << std::endl; // 8
    std::cout << "Eng: " << eng << std::endl; // 485



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
