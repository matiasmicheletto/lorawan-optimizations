#define MANUAL "readme_gpprs.txt"
#define LOGFILE "summary.csv"

#include <cstring>
#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/optimization/random.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/ga.h"
#include "lib/optimization/siman.h"


int main(int argc, char **argv) {
    
    srand(time(NULL));

    Instance *l = 0;
    uint maxIters = 1e5;
    uint timeout = 3600;
    TunningParameters tp; // alpha, beta and gamma
    bool verbose = false; // Disable printing to terminal
    bool wst = false; // Disable XML wst file export
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
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--method") == 0) {
            if(i+1 < argc) {
                if(std::strcmp(argv[i+1], "RS") == 0)
                    method = 0;
                else if(std::strcmp(argv[i+1], "IRS") == 0)
                    method = 1;
                else if(std::strcmp(argv[i+1], "GA") == 0)
                    method = 2;
                else if(std::strcmp(argv[i+1], "NSGA") == 0)
                    method = 3;
                else if(std::strcmp(argv[i+1], "SA") == 0)
                    method = 4;
                else if(std::strcmp(argv[i+1], "GD") == 0)
                    method = 5;
                else if(std::strcmp(argv[i+1], "GGW") == 0)
                    method = 6;
                else if(std::strcmp(argv[i+1], "GE") == 0)
                    method = 7;
                else if(std::strcmp(argv[i+1], "GU") == 0)
                    method = 8;
                else if(std::strcmp(argv[i+1], "G2") == 0)
                    method = 9;
                else if(std::strcmp(argv[i+1], "G3") == 0)
                    method = 10;
                else if(std::strcmp(argv[i+1], "G4") == 0)
                    method = 11;
                else if(std::strcmp(argv[i+1], "G5") == 0)
                    method = 12;
                else if(std::strcmp(argv[i+1], "G6") == 0)
                    method = 13;
                else if(std::strcmp(argv[i+1], "G7") == 0)
                    method = 14;
                else if(std::strcmp(argv[i+1], "G8") == 0)
                    method = 15;
                else if(std::strcmp(argv[i+1], "G9") == 0)
                    method = 16;
                else if(std::strcmp(argv[i+1], "G10") == 0)
                    method = 17;
                else if(std::strcmp(argv[i+1], "G11") == 0)
                    method = 18;
                else if(std::strcmp(argv[i+1], "G12") == 0)
                    method = 19;
                else if(std::strcmp(argv[i+1], "G13") == 0)
                    method = 20;
                else 
                    std::cerr << "Unknown optimization method. Defaulting to RS" << std::endl;
            }else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            verbose = true;
        }
        if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--wst") == 0){
            wst = true;
        }
    }

    if(l == nullptr) printHelp(MANUAL);

    if(verbose) {    
        std::cout << "Input file loaded." << std::endl;
        std::cout << "GW Count: " << l->gwCount << std::endl;
        std::cout << "ED Count: " << l->edCount << std::endl; 
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
            results = randomSearch(l, o, maxIters, timeout, verbose, wst);    
            results.solverName = strdup("Random Search");
            break;
        }
        case 1: {
            results = improvedRandomSearch(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Improved Random Search");
            break;
        }
        case 2: {
            GAConfig gaconfig;
            gaconfig.maxgen = maxIters/gaconfig.popsize;
            gaconfig.timeout = timeout;
            results = ga(l, o, gaconfig, verbose, wst);
            results.solverName = strdup("Genetic Algorithm");
            break;
        }
        case 3: {
            GAConfig gaconfig;
            gaconfig.maxgen = maxIters/gaconfig.popsize;
            gaconfig.timeout = timeout;
            results = nsga(l, o, gaconfig, verbose, wst);
            results.solverName = strdup("Nondominated Sorting Genetic Algorithm");
            break;
        }
        case 4: {
            results = siman(l, o, maxIters, verbose, wst);
            results.solverName = strdup("Simulated Anealing");
            break;
        }
        case 5: {
            results = greedy(l, o, maxIters, timeout, verbose);
            results.solverName = strdup("Greedy (GD)");
            break;
        }
        case 11: {
            results = greedy4(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 4");
            break;
        }
        case 15: {
            results = greedy8(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 8");
            break;
        }
        /*
        case 6: {
            results = greedy1(l, o, MIN::GW, verbose, wst);
            results.solverName = strdup("Greedy GW Minimization");
            break;
        }
        case 7: {
            results = greedy1(l, o, MIN::E, verbose, wst);
            results.solverName = strdup("Greedy E Minimization");
            break;
        }
        case 8: {
            results = greedy1(l, o, MIN::UF, verbose, wst);
            results.solverName = strdup("Greedy UF Minimization");
            break;
        }
        case 9: {
            results = greedy2(l, o, verbose, wst);
            results.solverName = strdup("Greedy 2");
            break;
        }
        case 10: {
            results = greedy3(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 3");
            break;
        }
        case 12: {
            results = greedy5(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 5");
            break;
        }
        case 13: {
            results = greedy6(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 6");
            break;
        }
        case 14: {
            results = greedy7(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 7");
            break;
        }
        case 15: {
            results = greedy8(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 8");
            break;
        }
        case 16: {
            results = greedy9(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 9");
            break;
        }
        case 17: {
            results = greedy10(l, o, maxIters, timeout, verbose, wst);
            results.solverName = strdup("Greedy 10");
            break;
        }
        case 18: {
            results = greedy11(l, o, maxIters, timeout);
            results.solverName = strdup("Greedy 10");
            break;
        }
        case 19: {
            results = greedy12(l, o, maxIters, timeout);
            results.solverName = strdup("Greedy 10");
            break;
        }
        case 20: {
            results = greedy13(l, o, maxIters, timeout, verbose);
            results.solverName = strdup("Greedy 10");
            break;
        }
        */
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
