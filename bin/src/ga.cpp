#define MANUAL "readme_ga.txt"
#define LOGFILE "summary.csv"

#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/optimization/ga.h"


int main(int argc, char **argv) {

    #ifdef VERBOSE
        std::cout << std::endl << "Step 0 -- Load instance and optimization parameters" << std::endl;
    #endif

    Instance *l = nullptr;
    TunningParameters tp; // alpha, beta and gamma
    bool xml = false; // XML file export
    bool output = false; // Output to console

    char *xmlFileName;
    char *outputFileName;

    GAConfig *config = new GAConfig();
    config->populationSize = 50;
    config->maxGenerations = 50;
    config->mutationRate = 0.05;
    config->crossoverRate = 0.8; 
    config->elitismRate = 0.2;
    config->timeout = 360;
    config->stagnationWindow = 0.3;

    bool warmStart = false;

    OUTPUTFORMAT outputFormat = OUTPUTFORMAT::TXT;

    // Program arguments (h, f, t, i, a, b, g, m, c, e, w, o, p, q, x)
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
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pop") == 0) {
            // Read precomputed population
            warmStart = true;
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                config->timeout = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -t (--timeout)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iters") == 0) {
            if(i+1 < argc) 
                config->maxGenerations = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -i (--iters)" << std::endl;
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
        if(strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--qpop") == 0) {
            if(i+1 < argc) 
                config->populationSize = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -q (--qpop)" << std::endl;
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
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mut") == 0){
            if(i+1 < argc)
                config->mutationRate = atof(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -m (--mut)" << std::endl;
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
                std::cout << std::endl << "Error in argument -e (--elit)" << std::endl;
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
    GAFitness* gaFitness = new GAFitness(o);
    GeneticAlgorithm *ga = new GeneticAlgorithm(gaFitness, config); // Init without config

    if(warmStart){ // Read precomputed population
        struct Ed { // ED gene simplified
            unsigned int gw;
            unsigned int sf;
        };
        std::vector<Ed> network; // List of nodes
        std::vector<std::vector<Ed>> pop; // List of network configurations
        std::string input;
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

    GAResults results = ga->run();


    // Log results to summary file
    OptimizationResults oResults; // For logging results
    oResults.instanceName = l->getInstanceFileName();
    if(warmStart)
        oResults.solverName = strdup("GA - Warm start");
    else
        oResults.solverName = strdup("GA");
    oResults.tp = o->tp;
    oResults.execTime = results.elapsed;
    oResults.cost = results.bestFitnessValue;
    oResults.ready = true;
    AllocationChromosome* bestChromosome = (AllocationChromosome*) results.best;
    bestChromosome->getPhenotype(oResults.cost, oResults.gwUsed, oResults.energy, oResults.uf, oResults.feasible);
    logResultsToCSV(oResults, LOGFILE);


    // Extract gw and sf arrays
    std::vector<Gene*> genes = bestChromosome->getGenes();
    unsigned int edCount = genes.size();
    uint* gw = new uint[edCount];
    uint* sf = new uint[edCount];
    for (unsigned int i = 0; i < edCount; i++) {
        EdGene* gene = (EdGene*) genes[i];
        gw[i] = gene->getGW();
        sf[i] = gene->getSF();
    }

    if(xml){
        std::ofstream xmlOS(xmlFileName);
        o->exportWST(gw, sf, xmlOS);
    }

    if(output){
        Allocation allocation(l);
        for(unsigned int i = 0; i < edCount; i++)
            allocation.checkUFAndConnect(i, gw[i], sf[i]);
        EvalResults bestRes = o->eval(allocation);
        std::ofstream outputOS(outputFileName);
        o->printSolution(allocation, bestRes, false, false, false, outputOS);
    }else{
        results.best->printPhenotype();
        std::cout << "Total execution time = " << results.elapsed << " ms" << std::endl;
    }

    return 0;
}