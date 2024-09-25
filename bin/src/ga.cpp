#define MANUAL "readme_greedy.txt"
#define LOGFILE "summary.csv"

#include "lib/util/util.h"
#include "lib/model/instance.h"
#include "lib/model/objective.h"
#include "lib/custom_ga/ga.h"


int main(int argc, char **argv);


class EdGene : public Gene { // End device: has a GW and a SF
    public:
        EdGene(Objective* o, unsigned int index) : Gene() {
            this->o = o;
            this->index = index;
            randomize();
        }

        inline void randomize() override {
            Instance *l = o->getInstance();
            std::vector<uint> gwList = l->getGWList(index); // Valid gw for this ed
            const unsigned int gwIndex = rand() % gwList.size();
            gw = gwList[gwIndex]; // Pick random gw
            // Pick random SF from valid range
            const uint minSF = l->getMinSF(index, gw);
            const uint maxSF = l->getMaxSF(index);
            sf = minSF + rand() % (maxSF - minSF + 1);
        }

        inline void print() const override {
            std::cout << gw << "[" << sf << "] ";
        }

        inline void setValue(unsigned int gw, unsigned int sf) {
            this->gw = gw;
            this->sf = sf;
        }

        inline unsigned int getGW() const {
            return gw;
        }

        inline unsigned int getSF() const {
            return sf;
        }
    
    private:
        unsigned int gw;
        unsigned int sf;
        Objective* o;
        unsigned int index;
};

class AllocationChromosome : public Chromosome { // Network allocation
    public:
        AllocationChromosome(Objective* o) : Chromosome(o->getInstance()->edCount) {
            this->o = o;
            unsigned int edCount = o->getInstance()->edCount;
            for (unsigned int i = 0; i < edCount; i++) {
                genes.push_back(new EdGene(o, i));
            }
        }

        void printGenotype() const override {
            std::cout << "Genotype: ";
            for (Gene* gene : genes) {
                gene->print();
            }
            std::cout << std::endl;
        }

        void printPhenotype() const override {
            unsigned int edCount = genes.size();
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (unsigned int i = 0; i < edCount; i++) {
                EdGene* gene = (EdGene*) genes[i];
                gw[i] = gene->getGW();
                sf[i] = gene->getSF();
            }

            // Evaluate cost
            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            double cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);            
            /*
            std::cout << "  Cost: " << cost << std::endl;
            std::cout << "  GW used: " << gwCount << std::endl;
            std::cout << "  Energy: " << energy << std::endl;
            std::cout << "  Total UF: " << totalUF << std::endl;
            std::cout << "  Feasible: " << (feasible ? "Yes" : "No");
            */
            std::cout << "Cost=" << cost
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << totalUF
                << ")" << std::endl;
        }

        void clone(const Chromosome& other) { // Copy the genes from another chromosome
            std::vector<Gene*> otherGenes = other.getGenes();
            std::vector<Gene*> thisGenes = getGenes();
            for (unsigned int i = 0; i < otherGenes.size(); i++) {
                EdGene *thisGene = (EdGene*) thisGenes[i];
                EdGene *otherGene = (EdGene*) otherGenes[i];
                unsigned int gw = otherGene->getGW();
                unsigned int sf = otherGene->getSF();
                thisGene->setValue(gw, sf);
            }
        }
    
    private:
        Objective* o;
};


class NetworkFitness : public Fitness { // Cost fitness function
    public:
        NetworkFitness(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        double evaluate(const Chromosome *chromosome) const override {
            // Get gw and sf arrays
            std::vector<Gene*> genes = chromosome->getGenes();
            unsigned int edCount = genes.size();
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (unsigned int i = 0; i < edCount; i++) {
                EdGene* gene = (EdGene*) genes[i];
                gw[i] = gene->getGW();
                sf[i] = gene->getSF();
            }

            // Evaluate cost
            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            double cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);
            // Inverse cost for maximization
            double inverseCost = feasible ? 1e4/cost : __DBL_MIN__;
            return inverseCost;
        }

        Chromosome* generateChromosome() const override {
            const unsigned int edCount = o->getInstance()->edCount;
            AllocationChromosome* ch = new AllocationChromosome(o);
            ch->fitness = evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};


int main(int argc, char **argv) {

    srand(time(nullptr));

    #ifdef VERBOSE
        std::cout << std::endl << "Step 0 -- Load instance and optimization parameters" << std::endl;
    #endif

    Instance *l = nullptr;
    TunningParameters tp; // alpha, beta and gamma
    bool xml = false; // XML file export
    bool output = false; // Output to console

    char *xmlFileName;
    char *outputFileName;

    GAConfig config;
    config.populationSize = 100;
    config.maxGenerations = 50;
    config.mutationRate = 0.1;
    config.crossoverRate = 0.8; 
    config.elitismRate = 0.05;
    config.timeout = 360;
    config.stagnationThreshold = 0;

    
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
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if(i+1 < argc) 
                config.timeout = atoi(argv[i+1]);
            else{
                printHelp(MANUAL);
                std::cout << std::endl << "Error in argument -t (--timeout)" << std::endl;
            }
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iters") == 0) {
            if(i+1 < argc) 
                config.maxGenerations = atoi(argv[i+1]);
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
    config.fitnessFunction = new NetworkFitness(o);

    GeneticAlgorithm *ga = new GeneticAlgorithm(config);
    GAResults results = ga->run();

    //results.print();
    results.best->printPhenotype();
    std::cout << "Total execution time = " << results.elapsed << " ms" << std::endl;

    return 0;
}