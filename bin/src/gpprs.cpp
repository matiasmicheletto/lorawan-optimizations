#define MANUAL "readme_gpprs.txt"
#define LOGFILE "summary.csv"

#include <cstring>
#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/optimization/random.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/greedy2.h"
#include "lib/optimization/ga.h"
#include "lib/optimization/results.h"

int main(int argc, char **argv) {
    
    srand(time(NULL));

    Instance *l = 0;
    unsigned long maxIters = 1e5;
    TunningParameters tp; // alpha, beta and gamma
    bool verbose = false; // Disable printing to terminal
    int method = 0; // Default is random search
    
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
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--method") == 0) {
            if(i+1 < argc) {
                if(std::strcmp(argv[i+1], "RS") == 0)
                    method = 0;
                else if(std::strcmp(argv[i+1], "IRS") == 0)
                    method = 1;
                else if(std::strcmp(argv[i+1], "GA") == 0)
                    method = 2;
                else if(std::strcmp(argv[i+1], "GGW") == 0)
                    method = 3;
                else if(std::strcmp(argv[i+1], "GE") == 0)
                    method = 4;
                else if(std::strcmp(argv[i+1], "GU") == 0)
                    method = 5;
                else if(std::strcmp(argv[i+1], "G2") == 0)
                    method = 6;
                else if(std::strcmp(argv[i+1], "NSGA") == 0)
                    method = 7;
                else 
                    std::cerr << "Unknown optimization method. Defaulting to RS" << std::endl;
            }else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            verbose = true;
        }
    }

    if(l == nullptr) printHelp(MANUAL);

    if(verbose) {    
        std::cout << "Input file loaded." << std::endl;
        std::cout << "GW Count: " << l->getGWCount() << std::endl;
        std::cout << "ED Count: " << l->getEDCount() << std::endl; 
        std::cout << "Tunning parameters:" << std::endl;
        std::cout << "  Alpha: " << tp.alpha << std::endl;
        std::cout << "  Beta: " << tp.beta << std::endl;
        std::cout << "  Gamma: " << tp.gamma << std::endl;
        std::cout << "Optimization method: " << method << std::endl << std::endl;
    }

    Objective *o = new Objective(l, tp);
    OptimizationResults results;

    switch (method) {
        case 0: {
            results = randomSearch(l, o, maxIters, verbose);    
            results.solverName = strdup("Random Search");
            break;
        }
        case 1: {
            results = improvedRandomSearch(l, o, maxIters, verbose);
            results.solverName = strdup("Improved Random Search");
            break;
        }
        case 2: {
            GAConfig gaconfig;
            gaconfig.maxgen = maxIters/gaconfig.popsize;
            results = ga(l, o, gaconfig, verbose);
            results.solverName = strdup("Genetic Algorithm");
            break;
        }
        case 3: {
            results = greedy(l, o, MIN::GW, verbose);
            results.solverName = strdup("Greedy GW Minimization");
            break;
        }
        case 4: {
            results = greedy(l, o, MIN::E, verbose);
            results.solverName = strdup("Greedy E Minimization");
            break;
        }
        case 5: {
            results = greedy(l, o, MIN::UF, verbose);
            results.solverName = strdup("Greedy UF Minimization");
            break;
        }
        case 6: {
            results = greedy2(l, o, verbose);
            results.solverName = strdup("Greedy 2");
            break;
        }
        case 7: {
            GAConfig gaconfig;
            gaconfig.maxgen = maxIters/gaconfig.popsize;
            results = nsga(l, o, gaconfig, verbose);
            results.solverName = strdup("Nondominated Sorting Genetic Algorithm");
            break;
        }
        default: {
            if(verbose)
                std::cerr << "Error: Unknown optimization method." << std::endl;
            exit(1);
            break;
        }
    }
    
    if(results.ready) {
        results.instanceName = l->getInstanceFileName();
        logResultsToCSV(results, LOGFILE);
    }
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
