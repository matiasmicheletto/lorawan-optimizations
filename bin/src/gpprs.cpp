#define MANUAL "readme_gpprs.txt"

#include <cstring>

#include "lib/util/util.h"

#include "lib/instance/instance.h"
#include "lib/objective/objective.h"

#include "lib/optimization/random.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/ga.h"


int main(int argc, char **argv) {
    
    srand(time(NULL));

    Instance *l = 0;
    unsigned long maxIters = 1e5;
    double alpha = 1.0, beta = 1.0, gamma = 1.0;
    bool verbose = false;
    
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
                alpha = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                beta = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gamma") == 0){
            if(i+1 < argc)
                gamma = atof(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            verbose = true;
        }
    }

    if(l == 0) printHelp(MANUAL);

    
    std::cout << "Input file loaded." << std::endl;
    // l->printRawData();
    // std::cout << std::endl;
    std::cout << "GW Count: " << l->getGWCount() << std::endl;
    std::cout << "ED Count: " << l->getEDCount() << std::endl; 
    std::cout << "Tunning parameters:" << std::endl;
    std::cout << "  Alpha: " << alpha << std::endl;
    std::cout << "  Beta: " << beta << std::endl;
    std::cout << "  Gamma: " << gamma << std::endl << std::endl;
    std::cout << "Optimization method: " << "all" << std::endl;
    std::cout << "Verbose mode: " << (verbose ? "true" : "false") << std::endl;


    Objective *o = new Objective(l, {alpha, beta, gamma});

    /*
    std::cout << std::endl << "-------------- RS ----------------" << std::endl << std::endl;
    randomSearch(l, o, maxIters, verbose);
    
    std::cout << std::endl << "-------------- IRS ---------------" << std::endl << std::endl;
    improvedRandomSearch(l, o, maxIters, verbose);
    
    std::cout << std::endl << "------------- Greedy -------------" << std::endl << std::endl;
    greedy(l, o, maxIters, verbose);
    */

    std::cout << std::endl << "--------------- GA ---------------" << std::endl << std::endl;
    ga(l, o);
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
