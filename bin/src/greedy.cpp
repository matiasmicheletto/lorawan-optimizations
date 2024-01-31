#define MANUAL "readme_greedy.txt"
#define LOGFILE "summary.csv"

#include <cstring>
#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/random.h"
#include "lib/optimization/ga.h"
#include "lib/optimization/siman.h"

auto getElapsed(std::chrono::_V2::system_clock::time_point start) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start).count();
    return elapsedSeconds;
}

auto getElapsedMs(std::chrono::_V2::system_clock::time_point start) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - start).count();
    return elapsedMs;
}

bool isTimeout(std::chrono::_V2::system_clock::time_point start, uint timeout) {
    return getElapsed(start) >= timeout;
}



int main(int argc, char **argv) {
    
    srand(time(NULL));
    std::random_device rd;
    std::mt19937 gen(rd());

    #ifdef VERBOSE
        std::cout << std::endl << "Phase 0 -- Load instance and optimization parameters" << std::endl;
    #endif

    Instance *l = 0;
    uint stallMax = 1; // %
    uint timeout = 60;
    TunningParameters tp; // alpha, beta and gamma
    bool xml = false; // XML file export
    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc)
                l = new Instance(argv[i + 1]);
            else{
                std::cerr << "Error in argument -f (--file)" << std::endl;
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stall") == 0) {
            if(i+1 < argc) 
                stallMax = atoi(argv[i+1]);
            else{
                std::cerr << "Error in argument -s (--stall)" << std::endl;
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                timeout = atoi(argv[i+1]);
            else{
                std::cerr << "Error in argument -t (--timeout)" << std::endl;
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0){
            if(i+1 < argc)
                tp.alpha = atof(argv[i+1]);
            else{
                std::cerr << "Error in argument -a (--alpha)" << std::endl;
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                tp.beta = atof(argv[i+1]);
            else{
                std::cerr << "Error in argument -b (--beta)" << std::endl;
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gamma") == 0){
            if(i+1 < argc)
                tp.gamma = atof(argv[i+1]);
            else{
                std::cerr << "Error in argument -g (--gamma)" << std::endl;
                printHelp(MANUAL);
            }
        }
        xml = strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--xml") == 0;
    }

    if(l == nullptr){
        std::cerr << "Invalid instance model. Check input file validity." << std::endl;
        printHelp(MANUAL);
    }
    
    #ifdef VERBOSE
        std::cout << "Timeout: " << timeout << " seconds." << std::endl;
        std::cout << "Stagnation limit: " << stallMax << " %" << std::endl;
        std::cout << "Input file loaded." << std::endl;
        std::cout << "GW Count: " << l->gwCount << std::endl;
        std::cout << "ED Count: " << l->edCount << std::endl; 
        std::cout << "Tunning parameters:" << std::endl;
        std::cout << "  Alpha: " << tp.alpha << std::endl;
        std::cout << "  Beta: " << tp.beta << std::endl;
        std::cout << "  Gamma: " << tp.gamma << std::endl;
    #endif

    Objective *o = new Objective(l, tp);

    auto start = std::chrono::high_resolution_clock::now();




    #ifdef VERBOSE
        std::cout << std::endl << "Phase 1 -- Build clusters (reacheability tensor) -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    std::vector<std::vector<std::vector<uint>>> clusters(6); // Clusters tensor (SF x GW x ED)
    int minCoverage = 7; // Min SF value with full coverage
    for (uint s = 7; s <= 12; s++) {
        for (uint g = 0; g < l->gwCount; g++)
            clusters[s - 7].push_back(l->getAllEDList(g, s));

        // Check if SF has coverage
        for (uint e = 0; e < l->edCount; e++) {
            bool hasGW = false;
            for (uint g = 0; g < l->gwCount; g++) {
                auto it = std::find(clusters[s - 7][g].begin(), clusters[s - 7][g].end(), e);
                hasGW = (it != clusters[s - 7][g].end());
                if(hasGW) break; // Next ED
            }
            if (!hasGW){
                #ifdef VERBOSE    
                    std::cout << "No coverage for SF " << s << ": ED " << e << " cannot be assigned to any GW." << std::endl;
                #endif
                minCoverage = s+1;
                break;
            }
        }
    }
    if(minCoverage == 13){ // If not coverage reached, exit
        std::cerr << "System is not feasible (no coverage for SF 12). Exiting program..." << std::endl;
        exit(1);
    }



    #ifdef VERBOSE
        std::cout << std::endl << "Phase 2 -- Find and allocate essential nodes -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    Allocation bestAllocation(l); // Allocation (initially empty)
    // Essential and non essential gw
    std::vector<uint> essGW;
    std::vector<uint> nEssGW;
    // Non essential nodes
    std::vector<uint> nEssED;
    // Find essential gws and allocate essential nodes
    for(uint g = 0; g < l->gwCount; g++){
        bool gIsEssential = false;
        const std::vector<uint> edsOfCurrentGW = l->getAllEDList(g, 12);
        for(uint i = 0; i < edsOfCurrentGW.size(); i++){ 
            const uint e = edsOfCurrentGW[i];
            if(l->getGWList(e).size() == 1){ // GW g is essential for node e
                gIsEssential = true;
                auto it = std::find(essGW.begin(), essGW.end(), g);
                if(it == essGW.end())
                    essGW.push_back(g);
                // Try to allocate essential ed to essential gw
                if(!bestAllocation.checkUFAndConnect(e, g)){
                    std::cerr << "ED " << e << " cannot be connected to essential GW " << g << std::endl;
                    std::cerr << "Not enough UF available:" << std::endl;
                    bestAllocation.uf[g].printUFValues();
                    std::cerr << "Unfeasible system. Exiting program..." << std::endl;
                    exit(1);
                }
            }else{ // Add non essential node to list (to allocate later)
                auto it = std::find(nEssED.begin(), nEssED.end(), e);
                if(it == nEssED.end())
                    nEssED.push_back(e);
            }
        }
        if(!gIsEssential)
            nEssGW.push_back(g);
        #ifdef VERBOSE
        else
            std::cout << "Essential GW " << g << " has " << edsOfCurrentGW.size() << " reachable nodes, " << bestAllocation.connectedCount << " total nodes connected." << std::endl;
        #endif
    }
    #ifdef VERBOSE
        std::cout << std::endl << "Essential GWs: " << essGW.size() << " (of " << l->gwCount << "):" << std::endl;
        for(uint i = 0; i < essGW.size(); i++)
            std::cout << essGW[i] << " ";
        std::cout << std::endl << std::endl;
        std::cout << "Total connected nodes to essential GWs: " << l->edCount - nEssED.size() << " (" << nEssED.size() << " left)." << std::endl;
    #endif



    #ifdef VERBOSE
        std::cout << std::endl << "Phase 3 -- Sort nodes by reachable gateways -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif 
    std::vector<uint> reachGW(nEssED.size());
    for (uint ei = 0; ei < nEssED.size(); ei++) {
        const uint e = nEssED[ei];
        reachGW[ei] = l->getGWList(e).size(); // Number of gws
    }
    std::sort(
        nEssED.begin(),
        nEssED.end(),
        [&reachGW](const uint & a, const uint & b) {
            return reachGW[a] < reachGW[b];
        }
    );
    #ifdef VERBOSE
        std::cout << "Sorting finished." << std::endl;
    #endif 



    #ifdef VERBOSE
        std::cout << std::endl << "Phase 4 -- Allocation of non essential nodes -- elapsed = " << getElapsed(start) << " sec." << std::endl;
        std::cout << "Start allocation from SF " << minCoverage << std::endl;
    #endif

    double minimumCost = __DBL_MAX__;
    for(uint s = minCoverage; s <= 12; s++){

        int exitCond = 0; // 1 -> timeout, 2 -> stagnation, 3 -> no changes
        uint totalLoops = 0;
        uint loopsWithoutImprovement = 1;
        const uint maxLoops = 1000; // Stop after this number of loops without improvement
        const Allocation essentials = bestAllocation;
        while(exitCond == 0) { // Until exit condition is met

            if (isTimeout(start, timeout)) { // Check time limit
                #ifdef VERBOSE
                    std::cout << std::endl << "Time limit reached in allocation stage." << std::endl;
                #endif
                exitCond = 1;
                break;
            }

            // Shuffle list of essential and non-essential gws
            std::shuffle(essGW.begin(), essGW.end(), gen); 
            std::shuffle(nEssGW.begin(), nEssGW.end(), gen);
            
            // Start allocation of non essential EDs (essential gws first)
            Allocation tempAlloc = essentials;
            for (uint ei = 0; ei < nEssED.size(); ei++) {    
                const uint e = nEssED[ei];
                for (uint gi = 0; gi < l->gwCount; gi++) {
                    const uint g = gi < essGW.size() ? essGW[gi] : nEssGW[gi - essGW.size()];
                    // Check if ED e can be connected to GW g
                    auto it = std::find(clusters[s - 7][g].begin(), clusters[s - 7][g].end(), e);
                    if (it != clusters[s - 7][g].end()) 
                        if(tempAlloc.checkUFAndConnect(e, g)) // If reachable, check uf and then connect
                            break; 
                }
                if(!tempAlloc.connected[e]) break;
            }

            // If all nodes connected, eval solution
            if(tempAlloc.connectedCount == l->edCount){ 
                EvalResults res = o->eval(tempAlloc);
                if(res.feasible && res.cost < minimumCost){
                    const double diff = minimumCost - res.cost;
                    const double improvement = round((diff/loopsWithoutImprovement)/(double) res.cost * 100);
                    #ifdef VERBOSE
                        std::cout << std::endl
                            << "Iteration " << totalLoops << ". New best for SF = " << s 
                            << " after " << loopsWithoutImprovement << " attempts. " 
                            << "Improvement = " << improvement << "%" << std::endl;
                        std::cout << "Prev Cost=" << minimumCost << ", New ";
                        o->printSolution(tempAlloc, res, false, false, false);
                    #endif
                    loopsWithoutImprovement = 1;
                    minimumCost = res.cost;
                    bestAllocation = tempAlloc;
                    if(improvement < (double)stallMax){
                        exitCond = 2;
                        #ifdef VERBOSE
                            std::cout << std::endl << "Stagnation at iteration " << totalLoops << std::endl;
                        #endif
                    }
                }else{
                    loopsWithoutImprovement++;
                    if(loopsWithoutImprovement > maxLoops){
                        exitCond = 3;
                        #ifdef VERBOSE
                            std::cout << std::endl << "Stagnation after " << maxLoops << " loops" << std::endl;
                        #endif
                    }
                }
            }
            #ifdef VERBOSE
            else // There are not connected nodes
                std::cout << "Attempt " << totalLoops << ", connected nodes: " << tempAlloc.connectedCount << " (out of " << l->edCount << ")" << std::endl;
            #endif

            totalLoops++; 
        }
        if(exitCond != 0) break;
    }
    // Eval objective function (to get number of GWs)
    EvalResults bestRes = o->eval(bestAllocation);
    if(!bestRes.feasible){
        std::cerr << std::endl << "System not feasible after allocation of non essential nodes. Exiting program..." << std::endl;
        exit(1);
    }


    #ifdef VERBOSE
        std::cout << std::endl << "Phase 5 -- Reallocation -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    Allocation tempAlloc = bestAllocation;

    // Sort non essential GWs by number of EDs
    std::vector<uint> usedGWList;
    std::vector<std::vector<uint>> edsOfGW(bestRes.gwUsed); 
    for (uint e = 0; e < l->edCount; e++) { // Transverse all nodes
        const uint g = tempAlloc.gw[e];        
        auto it = std::find(usedGWList.begin(), usedGWList.end(), g); // Find gw of ED e
        if (it != usedGWList.end()) { // If gw already in list, add its ED (e)
            uint gwIndex = std::distance(usedGWList.begin(), it);
            edsOfGW[gwIndex].push_back(e); // Add ED to GW
        }else{ // If not, add
            usedGWList.push_back(g); // Add gw index to list
            edsOfGW[usedGWList.size() - 1].push_back(e); // Add first ED to gw
        }
    }

    // Sort indirection array in ascending order of number of EDs
    std::vector<uint> indirection(bestRes.gwUsed);
    std::sort(
        indirection.begin(),
        indirection.end(),
        [&edsOfGW](const uint & a,
            const uint & b) {
            return edsOfGW[a].size() < edsOfGW[b].size();
        }
    );

    // Sort non essential GWs arrays by number of EDs
    std::vector<uint> sortedGWList(bestRes.gwUsed);
    std::vector<std::vector<uint>> sortedGWEDs(bestRes.gwUsed);
    for (uint gi = 0; gi < bestRes.gwUsed; gi++) {
        sortedGWList[gi] = usedGWList[indirection[gi]];
        sortedGWEDs[gi] = edsOfGW[indirection[gi]];
    }
    // Start from first GW and try to reallocate all of its EDs to any of the following
	const uint nEssGWUsed = bestRes.gwUsed - essGW.size();
    for (uint gi = 0; gi < nEssGWUsed; gi++) {
        uint g1 = sortedGWList[gi];
        uint ei = 0;
        while (ei < sortedGWEDs[gi].size()) { // For each ED of GW g
            uint e = sortedGWEDs[gi][ei]; // Number of ED
            std::vector<uint> availablesGWs = l->getSortedGWList(e); // List of GW in range of this ED
            if (availablesGWs.size() > 1) {
                for (uint gi2 = 0; gi2 < bestRes.gwUsed; gi2++) { // Search if possible to allocate to another used GW
                    const uint g2 = sortedGWList[gi2];
                    if (g1 != g2) {
                        auto it = std::find(availablesGWs.begin(), availablesGWs.end(), g2);
                        if (it != availablesGWs.end()) { // g2 is in available list, check if enough UF
                            if(tempAlloc.checkUFAndMove(e, g2)){ // If moved e, remove from gw and sort arrays again
                                sortedGWEDs[gi].erase(std::remove(sortedGWEDs[gi].begin(), sortedGWEDs[gi].end(), e), sortedGWEDs[gi].end()); // Remove ED e from GW g
                                std::sort(
                                    indirection.begin(),
                                    indirection.end(),
                                    [&edsOfGW](const uint & a,
                                        const uint & b) {
                                        return edsOfGW[a].size() < edsOfGW[b].size();
                                    }
                                );
                                for (uint gii = 0; gii < nEssGWUsed; gii++) {
                                    sortedGWList[gii] = usedGWList[indirection[gii]];
                                    sortedGWEDs[gii] = edsOfGW[indirection[gii]];
                                }
                                #ifdef VERBOSE
                                    std::cout << "Reallocated ED " << e << ": GW " << g1 << " --> " << g2 << ", with new SF: " << tempAlloc.sf[e] << std::endl;
                                #endif
                                break;
                            }
                        }
                    }
                }
            }
            ei++;
        }
    }

    EvalResults tempRes = o->eval(tempAlloc);
    if(tempRes.cost < bestRes.cost){
        #ifdef VERBOSE
            std::cout << "Cost change after reallocation:" << std::endl;
            std::cout << "GW = " << bestRes.gwUsed << " --> " << tempRes.gwUsed << std::endl;
            std::cout << "E = " << bestRes.energy << " --> " << tempRes.energy << std::endl;
        #endif
        bestAllocation = tempAlloc;
        bestRes = tempRes;
    }else{
        #ifdef VERBOSE
            std::cout << "No improvement after reallocation" << std::endl;
        #endif
    }


    #ifdef VERBOSE
        std::cout << std::endl << "Phase 6 -- Print results -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    // Print results and exit
    o->printSolution(bestAllocation, bestRes, false, true, true);

    OptimizationResults results;
    results.solverName = strdup("Greedy");
    results.execTime = getElapsedMs(start);
    results.ready = true;
    results.instanceName = l->getInstanceFileName();
    logResultsToCSV(results, LOGFILE);

    if(xml) o->exportWST(bestAllocation.gw.data(), bestAllocation.sf.data());
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
