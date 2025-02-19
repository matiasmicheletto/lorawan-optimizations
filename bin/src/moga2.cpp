#define MANUAL "readme_ga.txt"
#define LOGFILE "summary.csv"

#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/optimization/ga.h"
#include "lib/optimization/fitness/ga_fitness_gw.h"
#include "lib/optimization/fitness/ga_fitness_e.h"
#include "lib/optimization/fitness/ga_fitness_uf.h"

int main(int argc, char **argv) {

    #ifdef VERBOSE
        std::cout << std::endl << "Step 0 -- Load instance and optimization parameters" << std::endl;
    #endif

    Instance *l = nullptr;
    TunningParameters tp; // alpha, beta and gamma

    GAConfig *config = new GAConfig();
    config->populationSize = 50;
    config->maxGenerations = 50;
    config->mutationRate = 0.05;
    config->crossoverRate = 0.8; 
    config->elitismRate = 0.2;
    config->crossoverMethod = CROSS_METHOD::SINGLE_POINT;
    config->timeout = 360;
    config->stagnationWindow = 0.3;

    bool warmStart = false;

    OUTPUTFORMAT outputFormat = OUTPUTFORMAT::TXT;

    int objective = -1;

    char *filename = nullptr;

    
    for(int i = 0; i < argc; i++) {    
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
        if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cross") == 0){
            if(i+1 < argc)
                config->crossoverRate = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -c (--cross)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--elite") == 0){
            if(i+1 < argc)
                config->elitismRate = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -e (--elite)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc){
                filename = argv[i + 1];
                l = new Instance(argv[i + 1]);
            }else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -f (--file)" << std::endl;
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
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iters") == 0) {
            if(i+1 < argc) 
                config->maxGenerations = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -i (--iters)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--crossmethod") == 0){
            if(i+1 < argc){
                if(std::strcmp(argv[i+1], "UNIFORM") == 0)
                    config->crossoverMethod = CROSS_METHOD::C_UNIFORM;
                if(std::strcmp(argv[i+1], "SINGLE_POINT") == 0)
                    config->crossoverMethod = CROSS_METHOD::SINGLE_POINT;
                if(std::strcmp(argv[i+1], "DOUBLE_POINT") == 0)
                    config->crossoverMethod = CROSS_METHOD::DOUBLE_POINT;
            }else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -l (--log)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mut") == 0){
            if(i+1 < argc)
                config->mutationRate = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -m (--mut)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pop") == 0) {
            // Read precomputed population
            warmStart = true;
        }
        if(strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--qpop") == 0) {
            if(i+1 < argc) 
                config->populationSize = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -q (--qpop)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stag") == 0) {
            if(i+1 < argc) 
                config->stagnationWindow = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -s (--stag)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                config->timeout = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -t (--timeout)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--xformat") == 0){
            if(i+1 < argc){
                if(std::strcmp(argv[i+1], "HTML") == 0)
                    outputFormat = OUTPUTFORMAT::HTML;
                if(std::strcmp(argv[i+1], "TXT") == 0)
                    outputFormat = OUTPUTFORMAT::TXT;
                if(std::strcmp(argv[i+1], "SVG") == 0)
                    outputFormat = OUTPUTFORMAT::SVG;
                if(std::strcmp(argv[i+1], "CSV") == 0)
                    outputFormat = OUTPUTFORMAT::CSV;
            }else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-z") == 0 || strcmp(argv[i], "--zobj") == 0){
            if(i+1 < argc){
                if(std::strcmp(argv[i+1], "GW") == 0)
                    objective = 0;
                if(std::strcmp(argv[i+1], "E") == 0)
                    objective = 1;
                if(std::strcmp(argv[i+1], "UF") == 0)
                    objective = 2;
            }else
                printHelp(MANUAL);
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
    GAFitnessGW* gaFitnessGW = new GAFitnessGW(o);
    GAFitnessE* gaFitnessE = new GAFitnessE(o);
    GAFitnessUF* gaFitnessUF = new GAFitnessUF(o);

    GeneticAlgorithm *ga;

    // Print file name
    if(warmStart)
        std::cout << "MOGA2WS,";
    else
        std::cout << "MOGA2,";
    std::cout << extractFileName(filename) << ",";

    switch(objective){
        case -1:
            std::cerr << "Error: No objective function specified." << std::endl;
            exit(1);
        case 0:
            std::cout << "GW,";
            ga = new GeneticAlgorithm(gaFitnessGW, config); // Init without config
            break;
        case 1:
            std::cout << "E,";
            ga = new GeneticAlgorithm(gaFitnessE, config); // Init without config
            break;
        case 2:
            std::cout << "UF,";
            ga = new GeneticAlgorithm(gaFitnessUF, config); // Init without config
            break;
    }
    
    if(warmStart){ // Read precomputed population
        struct Ed { // ED gene simplified
            unsigned int gw;
            unsigned int sf;
        };
        std::vector<Ed> network; // List of nodes
        std::vector<std::vector<Ed>> pop; // List of network configurations
        std::string input;
        if(outputFormat != OUTPUTFORMAT::CSV)
            std::cout << "Reading population..." << std::endl;
        unsigned int popsize = 0;
        while(std::cin >> input) {
            if(input != "--"){
                unsigned int gw = std::stoi(input);
                unsigned int sf;
                std::cin >> sf;
                Ed ed = {gw, sf};
                network.push_back(ed);
            }else{
                popsize++;
                pop.push_back(network);
                network.clear();
            }
        }
        if(outputFormat != OUTPUTFORMAT::CSV)
            std::cout << "Population read. Size: " << popsize << std::endl;

        // Build population
        std::vector<Chromosome*> population;
        for(uint k = 0; k < pop.size(); k++){ // For each network config (chromosome)
            AllocationChromosome* ch = new AllocationChromosome(o);
            for(uint j = 0; j < pop[k].size(); j++){ // For each node (gene)
                ch->setGeneValue(j, pop[k][j].gw, pop[k][j].sf);
            }
            population.push_back(ch);
        }

        ga->setPopulation(population);
    }

    // Print configuration parameters
    /*
    std::cout << std::endl << "Program parameters: ";
    std::cout << config->populationSize << ",";
    std::cout << config->maxGenerations << ",";
    std::cout << config->mutationRate << ",";
    std::cout << config->crossoverRate << ",";
    std::cout << config->elitismRate << ",";
    std::cout << config->crossoverMethod << ",";
    std::cout << config->timeout << ",";
    std::cout << config->stagnationWindow << ",";
    std::cout << std::endl;
    */

    GAResults results = ga->run();
    results.outputFormat = outputFormat;
    results.print();

    return 0;
}