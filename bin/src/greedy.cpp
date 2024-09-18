#define MANUAL "readme_greedy.txt"
#define LOGFILE "summary.csv"

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
    return getElapsed(start) >= (int64_t)timeout;
}



int main(int argc, char **argv) {
    
    srand(time(NULL));
    std::random_device rd;
    std::mt19937 gen(rd());

    #ifdef VERBOSE
        std::cout << std::endl << "Step 0 -- Load instance and optimization parameters" << std::endl;
    #endif

    Instance *l = nullptr;
    double minImprovement = 1.0; // %
    uint timeout = 60;
    uint maxIterations = 10; // Stop after this number of loops without improvement
    TunningParameters tp; // alpha, beta and gamma
    bool xml = false; // XML file export
    bool output = false; // Output to console

    char *xmlFileName;
    char *outputFileName;

    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc)
                l = new Instance(argv[i + 1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -f (--file)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stall") == 0) {
            if(i+1 < argc) 
                minImprovement = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -s (--stall)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                timeout = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -t (--timeout)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iters") == 0) {
            if(i+1 < argc) 
                maxIterations = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -i (--iters)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0){
            if(i+1 < argc)
                tp.alpha = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -a (--alpha)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                tp.beta = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -b (--beta)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gamma") == 0){
            if(i+1 < argc)
                tp.gamma = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -g (--gamma)" << std::endl;
            }
        }
        //xml = strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--xml") == 0;
        if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--xml") == 0){
            if(i+1 < argc){
                xmlFileName = argv[i+1];
                xml = true;
            }else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -w (--xml)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0){
            if(i+1 < argc){
                outputFileName = argv[i+1];
                output = true;
            }else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -o (--output)" << std::endl;
            }
        }
    }

    if(l == nullptr){
        printHelp(MANUAL);
        std::cout << std::endl << "Invalid instance model. Must specify --file or -f argument and file name." << std::endl << std::endl;
        exit(1);
    }
    
    #ifdef VERBOSE
        std::cout << "Input file loaded." << std::endl;
        std::cout << "GW Count: " << l->gwCount << std::endl;
        std::cout << "ED Count: " << l->edCount << std::endl; 
        std::cout << "Exit conditions: " << std::endl;
            std::cout << " Timeout: " << timeout << " seconds." << std::endl;
            std::cout << " Stagnation limit: " << minImprovement << "\%" << std::endl;
            std::cout << " Max iterations: " << maxIterations << std::endl;
        std::cout << "Tunning parameters:" << std::endl;
            std::cout << "  Alpha: " << tp.alpha << std::endl;
            std::cout << "  Beta: " << tp.beta << std::endl;
            std::cout << "  Gamma: " << tp.gamma << std::endl;
    #endif

    Objective *o = new Objective(l, tp);

    auto start = std::chrono::high_resolution_clock::now();



    #ifdef VERBOSE
        std::cout << std::endl << "Step 2 -- Find and allocate essential nodes -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    Allocation bestAllocation(l); // Allocation (initially empty)
    // Essential and non essential gw
    std::unordered_set<uint> essGWSet; // Using set for unique values insertion
    std::vector<uint> nEssGW;
    // Non essential nodes
    std::vector<uint> nEssED;
    // Find essential gws and allocate essential nodes
    std::vector<uint> reachGW; // This structure is used in following step
    for(uint e = 0; e < l->edCount; e++){
        std::vector<uint> gwsOfE = l->getGWList(e);
        if(gwsOfE.size() == 1){ // e is essential, add gw to essential gw list
            essGWSet.insert(gwsOfE[0]);
            bestAllocation.checkUFAndConnect(e, gwsOfE[0]); // Connect node to essential gw
        }else{
            reachGW.push_back(gwsOfE.size()); // Add gw count of this non essential node
            nEssED.push_back(e);
        }
    }
    std::vector<uint> essGW(essGWSet.begin(), essGWSet.end());
    // Initialize list of non essential gws
    if(essGW.size() == 0){ // If no essential gws, initialize 
        nEssGW.resize(l->gwCount);
        std::iota(nEssGW.begin(), nEssGW.end(), 0);
    }else{ // Add the other gws to non essential list
        for(uint g = 0; g < l->gwCount; g++)
            if(std::find(essGW.begin(), essGW.end(), g) == essGW.end())
                nEssGW.push_back(g);
    }

    #ifdef VERBOSE
        std::cout << std::endl << "Essential GWs: " << essGW.size() << " (of " << l->gwCount << "):" << std::endl;
        for(uint i = 0; i < essGW.size(); i++)
            std::cout << essGW[i] << " ";
        std::cout << std::endl << std::endl;
        std::cout << "Total connected nodes to essential GWs: " << l->edCount - nEssED.size() << " (" << nEssED.size() << " left)." << std::endl;
    #endif



    #ifdef VERBOSE
        std::cout << std::endl << "Step 3 -- Sort nodes by reachable gateways -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif 
    std::vector<uint> indirection(nEssED.size());
    std::iota(indirection.begin(), indirection.end(), 0);
    std::sort(
        indirection.begin(),
        indirection.end(),
        [&reachGW](const uint & a, const uint & b) {
            return reachGW[a] < reachGW[b];
        }
    );

    #ifdef VERBOSE
        std::cout << "Sorting finished." << std::endl;
    #endif 



    #ifdef VERBOSE
        std::cout << std::endl << "Step 4 -- Allocation of non essential nodes -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif

    double minimumCost = __DBL_MAX__;
    bool timedout = false;
    const Allocation essentials = bestAllocation;
    for(uint s = 7; s <= 12; s++){

        std::vector<std::vector<uint>> cluster;
        // Build cluster for this SF
        for (uint g = 0; g < l->gwCount; g++)
            cluster.push_back(l->getAllEDList(g, s));
        // Check if SF has coverage
        bool hasCoverage = false;
        for (uint ei = 0; ei < nEssED.size(); ei++) {
            const uint e = nEssED[ei];
            for (uint g = 0; g < l->gwCount; g++) {
                auto it = std::find(cluster[g].begin(), cluster[g].end(), e);
                hasCoverage = (it != cluster[g].end());
                if(hasCoverage) break; // Next ED
            }
            if (!hasCoverage){
                #ifdef VERBOSE    
                    std::cout << "No coverage for SF " << s << ": ED " << e << " cannot be assigned to any GW." << std::endl;
                #endif
                break;
            }
        }
        if(!hasCoverage) continue; // Go to next SF
        #ifdef VERBOSE
            std::cout << "Coverage 100% reached at SF " << s << std::endl;
        #endif
        
        double improvement = 100.0;
        for(uint i = 0; i < maxIterations; i++) {

            if (isTimeout(start, (unsigned long int)timeout)) { // Check time limit
                #ifdef VERBOSE
                    std::cout << std::endl << "Time limit reached in allocation phase. Breaking phase." << std::endl;
                #endif
                timedout = true;
                break; // for iter and then for sf
            }
            
            // Shuffle list of essential and non-essential gws
            std::shuffle(essGW.begin(), essGW.end(), gen); 
            std::shuffle(nEssGW.begin(), nEssGW.end(), gen);
            
            // Start allocation of non essential EDs (essential gws first)
            Allocation tempAlloc = essentials;

            for (uint ei = 0; ei < nEssED.size(); ei++) {
                const uint e = nEssED[indirection[ei]]; // First nodes have less gws in range
                for (uint gi = 0; gi < l->gwCount; gi++) {
                    // g is the index of gw from the arrays essGW or nEssGW
                    const uint g = gi < essGW.size() ? essGW[gi] : nEssGW[gi - essGW.size()];
                    // Check if ED e can be connected to GW g
                    auto it = std::find(cluster[g].begin(), cluster[g].end(), e);
                    if (it != cluster[g].end())
                        if(tempAlloc.checkUFAndConnect(e, g)) // If reachable, check uf and then connect
                            break; // If connected, go to next ED
                }
                if(!tempAlloc.connected[e]) break; // If a node cannot be connected, break ED loop --> next iter
            }

            // If all nodes connected, eval solution
            if(tempAlloc.connectedCount == l->edCount){ 
                EvalResults res = o->eval(tempAlloc, false); // Use true to compute cost according to feasibility level
                if(res.feasible && res.cost < minimumCost){ // New minimum found
                    if(i > 0){ // Compute const improvement after first iteration
                        const double diff = minimumCost - res.cost;
                        improvement = round(diff/res.cost * 100);
                    }
                    #ifdef VERBOSE
                        std::cout << std::endl
                            << "Iteration " << i << ". New best for SF = " << s  
                            << ". Improvement = " << improvement << "\%" << std::endl;
                        if(i > 0) std::cout << "Prev Cost=" << minimumCost << ", New ";
                        o->printSolution(tempAlloc, res, false, false, false);
                    #endif
                    minimumCost = res.cost;
                    bestAllocation = tempAlloc;
                    if(improvement < minImprovement){
                        #ifdef VERBOSE
                            std::cout << std::endl 
                                    << "Improvement is below "  
                                    << minImprovement 
                                    << "\% in allocation phase. Breaking phase." 
                                    << std::endl;
                        #endif
                        break; // Next SF
                    }
                }
            }
            #ifdef VERBOSE
            else // There are not connected nodes
                std::cout << "SF " << s << ", iteration " << i << ", connected nodes: " << tempAlloc.connectedCount << " (out of " << l->edCount << ")" << std::endl;
            #endif
        }

        if(timedout) break; // Do not go to next SF
    }
    // Eval objective function (to get number of GWs)
    EvalResults bestRes = o->eval(bestAllocation);
    if(!bestRes.feasible){
        std::cout << std::endl << "System not feasible after allocation of non essential nodes. Exiting program..." << std::endl;
        std::cout << "Unfeasibility code: " << bestRes.unfeasibleCode << std::endl;
        exit(1);
    }


    #ifdef VERBOSE
        std::cout << std::endl << "Step 5 -- Reallocation -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    Allocation tempAlloc = bestAllocation;

    // Sort non essential GWs by number of EDs
    std::vector<uint> usedGWList;
    std::vector<std::vector<uint>> edsOfGW(bestRes.gwUsed); 
    for (uint e = 0; e < l->edCount; e++) { // Traverse all nodes
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
    std::vector<uint> indirection2(bestRes.gwUsed);
    std::iota(indirection2.begin(), indirection2.end(), 0);
    std::sort(
        indirection2.begin(),
        indirection2.end(),
        [&edsOfGW](const uint & a, const uint & b) {
            return edsOfGW[a].size() < edsOfGW[b].size();
        }
    );
    // Sort non essential GWs arrays by number of EDs
    std::vector<uint> sortedUsedGWList(bestRes.gwUsed);
    std::vector<std::vector<uint>> srtedGWbyEDs(bestRes.gwUsed);
    for (uint gi = 0; gi < bestRes.gwUsed; gi++) {
        const uint g = indirection2[gi];
        sortedUsedGWList[gi] = usedGWList[g];
        srtedGWbyEDs[gi] = edsOfGW[g];
    }

    // Start from first GW and try to connect all of its EDs to any of the following
    uint reallocationCount = 0;
	const uint nEssGWUsed = bestRes.gwUsed - essGW.size();
    for (uint gi = 0; gi < nEssGWUsed; gi++) {
        uint g1 = sortedUsedGWList[gi];    
        for (uint ei = 0; ei < srtedGWbyEDs[gi].size(); ei++) { // For each ED of GW g
            uint e = srtedGWbyEDs[gi][ei]; // Number of ED
            std::vector<uint> availablesGWs = l->getGWList(e); // List of GW in range of this ED
            for(long int gi2 = availablesGWs.size()-1; gi2 >= 0; gi2--){
                const uint g2 = availablesGWs[gi2];
                if(g2 != g1){
                    auto it = std::find(sortedUsedGWList.begin(), sortedUsedGWList.end(), g2);
                    if(it != sortedUsedGWList.end()){
                        if(tempAlloc.checkUFAndMove(e, g2)){ // If moved e, remove from gw and sort arrays again
                            #ifdef VERBOSE
                                std::cout << "Reallocated ED " << e << ": GW " << g1 << " --> " << g2 << ", with new SF: " << tempAlloc.sf[e] << std::endl;
                            #endif
                            reallocationCount++;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    EvalResults tempRes = o->eval(tempAlloc);
    if(tempRes.cost < bestRes.cost){
        #ifdef VERBOSE
            std::cout << std::endl << "Reallocated " << reallocationCount << " nodes" << std::endl;
            std::cout << "Cost change after reallocation:" << std::endl;
            std::cout << "  GW = " << bestRes.gwUsed << " --> " << tempRes.gwUsed << std::endl;
            std::cout << "  E = " << bestRes.energy << " --> " << tempRes.energy << std::endl;
        #endif
        bestAllocation = tempAlloc;
        bestRes = tempRes;
    }else{
        #ifdef VERBOSE
            std::cout << "No improvement after reallocation" << std::endl;
        #endif
    }



    #ifdef VERBOSE
        std::cout << std::endl << "Step 6 -- Print results -- elapsed = " << getElapsed(start) << " sec." << std::endl;
    #endif
    OptimizationResults results;
    results.instanceName = l->getInstanceFileName();
    results.solverName = strdup("Greedy (./greedy)");
    results.tp = o->tp;
    results.execTime = getElapsedMs(start);
    results.feasible = tempRes.feasible;
    results.cost = tempRes.cost;
    results.gwUsed = tempRes.gwUsed;
    results.energy = tempRes.energy;
    results.uf = tempRes.uf;
    results.ready = true;
    logResultsToCSV(results, LOGFILE);

    // Print results and exit
    #ifdef VERBOSE
        o->printSolution(bestAllocation, bestRes, true, true, true);
        std::cout << "Total execution time = " << results.execTime << " ms" << std::endl;
    #endif

    if(xml) {
        std::ofstream xmlOS(xmlFileName);
        o->exportWST(bestAllocation.gw.data(), bestAllocation.sf.data(), xmlOS);
    }

    if(output) {
        std::ofstream outputOS(outputFileName);
        o->printSolution(bestAllocation, bestRes, false, false, false, outputOS);
    }


    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
