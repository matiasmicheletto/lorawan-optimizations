#include "random.h"


OptimizationResults randomSearch(Instance* l, Objective* o, uint maxIters, uint timeout, bool verbose, bool wst){
    /* Fully random uniformly distributed solutions are generated */

    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose)
        std::cout << std::endl << "-------------- RS ----------------" << std::endl << std::endl;

    // Optimization variable
    uint* gw = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sf = (uint*) malloc( sizeof(uint) * l->getEDCount());
    // Best
    uint* gwBest = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sfBest = (uint*) malloc( sizeof(uint) * l->getEDCount());

    // Random generators use uniform distribution
    Uniform gwGenerator = Uniform(0, l->getGWCount());
    Uniform sfGenerator = Uniform(7, 12);

    double bestQ = __DBL_MAX__; // Cost minimization
    bool found = false;

    if(verbose)
        std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    for(uint k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(uint i = 0; i < l->getEDCount(); i++){
            gw[i] = gwGenerator.random();
            sf[i] = sfGenerator.random();
        }
        // Test generated solution
        uint gwCount, energy;
        double totalUF;
        bool feasible;
        const double q = o->eval(gw, sf, gwCount, energy, totalUF, feasible);

        if(feasible && q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            std::copy(gw, gw + l->getEDCount(), gwBest);
            std::copy(sf, sf + l->getEDCount(), sfBest);
            if(verbose){
                std::cout << "New best at iteration: " << k << std::endl;
                o->printSolution(gw, sf, false);
                std::cout << std::endl << std::endl;
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
        if (elapsedSeconds >= timeout) {
            if(verbose) std::cout << "Time limit reached." << std::endl;
            timedout = true;
            break;
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        
    if(verbose){    
        std::cout << "Random search finished, " << maxIters << " iterations executed in " << duration << " milliseconds." << std::endl;
        if(found){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
        }
    }

    if(wst) o->exportWST(gwBest, sfBest);

	OptimizationResults results;
    results.ready = found; // Set export flag to ready if solution was found
    if(found){
        results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
        results.execTime = duration;
        results.tp = o->tp;
    }

    // Release memory
    free(gw);
    free(sf);
    free(gwBest);
    free(sfBest);

    return results;
}

OptimizationResults improvedRandomSearch(Instance* l, Objective* o, uint maxIters, uint timeout, bool verbose, bool wst) {
    /* Random solutions are generated within feasible and more convenient SF and GW values */
    auto start = std::chrono::high_resolution_clock::now();
    bool timedout = false;

    if(verbose)
        std::cout << std::endl << "-------------- IRS ---------------" << std::endl << std::endl;

    // Optimization variable
    uint* gw = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sf = (uint*) malloc( sizeof(uint) * l->getEDCount());
    // Best
    uint* gwBest = (uint*) malloc( sizeof(uint) * l->getEDCount());
    uint* sfBest = (uint*) malloc( sizeof(uint) * l->getEDCount());

    Uniform uniform = Uniform(0.0, 1.0);

    double bestQ = __DBL_MAX__; // Cost minimization
    bool found = false;

    if(verbose)
        std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    for(uint k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(uint i = 0; i < l->getEDCount(); i++){
            std::vector<uint> gwList = l->getGWList(i); // Valid gw for this ed
            gw[i] = gwList[(uint)floor(uniform.random()*gwList.size())]; // Pick random gw
            // Pick random SF from valid range
            const uint minSF = l->getMinSF(i, gw[i]);
            const uint maxSF = l->getMaxSF(i);
            sf[i] = uniform.random()*(maxSF - minSF) + minSF;
        }
        // Test generated solution
        uint gwCount, energy;
        double totalUF;
        bool feasible;
        const double q = o->eval(gw, sf, gwCount, energy, totalUF, feasible);

        if(feasible && q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            std::copy(gw, gw + l->getEDCount(), gwBest);
            std::copy(sf, sf + l->getEDCount(), sfBest);
            if(verbose){
                std::cout << "New best at iteration: " << k << std::endl;
                o->printSolution(gw, sf, false);
                std::cout << std::endl;
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
        if (elapsedSeconds >= timeout) {
            if(verbose) std::cout << "Time limit reached." << std::endl;
            timedout = true;
            break;
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

    if(verbose){
        std::cout << "Improved random search finished, " << maxIters << " iterations executed in " << duration << " milliseconds." << std::endl;
        if(found){
            std::cout << "Result:" << std::endl;
            o->printSolution(gwBest, sfBest, true, true);
        }else{
            std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
        }
    }

    if(wst) o->exportWST(gwBest, sfBest);

    OptimizationResults results;
    results.ready = found; // Set export flag to ready if solution was found
    if(found){
        results.cost = o->eval(gwBest, sfBest, results.gwUsed, results.energy, results.uf, results.feasible);
        results.execTime = duration;
        results.tp = o->tp;
    }

    // Release memory
    free(gw);
    free(sf);
    free(gwBest);
    free(sfBest);

    return results;
}