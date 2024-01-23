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


int main(int argc, char **argv) {
    
    srand(time(NULL));

    Instance *l = 0;
    uint maxIters = 1e5;
    uint timeout = 3600;
    TunningParameters tp; // alpha, beta and gamma
    bool wst = false; // Disable XML wst file export
    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc)
                l = new Instance(argv[i + 1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iter") == 0) {
            if(i+1 < argc) 
                maxIters = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                timeout = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0){
            if(i+1 < argc)
                tp.alpha = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                tp.beta = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gamma") == 0){
            if(i+1 < argc)
                tp.gamma = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        wst = strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--wst") == 0;
    }

    if(l == nullptr) printHelp(MANUAL);
    
    #ifdef VERBOSE
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
        std::cout << std::endl << "Stage 1 -- Find essential nodes" << std::endl;
    #endif

    Allocation bestAllocation(l); // Allocation (empty)
    std::vector<uint> essGW;
    std::vector<uint> essED;
    std::vector<UtilizationFactor> essGWUF; // Utilization factors of essential GWs
    for(uint g = 0; g < l->gwCount; g++){
        bool isEssential = false;
        uint allocatedCount = 0;
        const std::vector<uint> edsOfCurrentGW = l->getAllEDList(g, 12);
        //  First pass: Search for essential nodes and allocate to GW
        for(uint i = 0; i < edsOfCurrentGW.size(); i++){ 
            const uint e = edsOfCurrentGW[i];
            if(l->getGWList(e).size() == 1){ // GW g is essential for node e
                isEssential = true;
                essED.push_back(e);
                // Add gw to esential list (if not in list)
                auto it = std::find(essGW.begin(), essGW.end(), g);
                uint essGWIndex;
                if(it == essGW.end()){
                    essGW.push_back(g);
                    essGWUF.push_back(UtilizationFactor());
                    essGWIndex = essGW.size()-1;
                }else
                    essGWIndex = std::distance(essGW.begin(), it);
                // Try to allocate essential ed to essential gw
                if(bestAllocation.checkBeforeConnect(e, g)){
                    essGWUF[essGWIndex] = bestAllocation.uf[g];
                    allocatedCount++;
                }else{
                    std::cerr << "ED " << e << " cannot be allocated to essential GW " << g << std::endl;
                    std::cerr << "Unfeasible system. Exiting program..." << std::endl;
                    exit(1);
                }
            }
        }
        #ifdef VERBOSE
            if(isEssential)
                std::cout << "Essential GW " << g << " has " << edsOfCurrentGW.size() << " reachable nodes, " << allocatedCount << " essential nodes allocated." << std::endl;
        #endif
    }

    #ifdef VERBOSE
        std::cout << std::endl << "Essential GWs: " << essGW.size() << " (of " << l->gwCount << ")" << std::endl;
        std::cout << "Total allocated nodes to essential GWs: " << essED.size() << std::endl;
    #endif

    //// TODO G4 y G8 (si hay esenciales)

    /// TODO G8 si no hay esenciales


    OptimizationResults results;
    results.solverName = strdup("Greedy");
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true;
    results.instanceName = l->getInstanceFileName();
    logResultsToCSV(results, LOGFILE);
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
