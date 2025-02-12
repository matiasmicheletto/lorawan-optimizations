#include "../custom_ga/uniform.h"
#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"

using namespace custom_ga;

class EdGene : public Gene { // End device: has a GW and a SF. Needs to know its index in the network
    public:
        EdGene(Objective* o, unsigned int index) : Gene() {
            this->o = o;
            this->index = index;
            randomize();
        }

        inline void randomize() override {
            Instance *l = o->getInstance();
            std::vector<uint> gwList = l->getGWList(index); // Valid gw for this ed
            // Pick random gw
            const unsigned int gwIndex = uniform.random(gwList.size());
            gw = gwList[gwIndex]; 
            // Pick random SF from valid range
            const uint minSF = l->getMinSF(index, gw);
            const uint maxSF = l->getMaxSF(index);
            //sf = minSF + rand() % (maxSF - minSF + 1);
            sf = uniform.random(minSF, maxSF);
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

class AllocationChromosome : public Chromosome { // Network allocation (GW and SF for each ED)
    public:
        AllocationChromosome(Objective* o) : Chromosome(o->getInstance()->edCount) {
            this->o = o;
            unsigned int edCount = o->getInstance()->edCount;
            for (unsigned int i = 0; i < edCount; i++) {
                genes.push_back(new EdGene(o, i));
            }
            this->setCrossoverMethod(&Chromosome::singlePointCrossover); // Default crossover method
        }

        std::string getName() const override {
            return "Allocation array";
        }

        void printGenotype() const override {
            std::cout << "Genotype: ";
            for (Gene* gene : genes) {
                gene->print();
            }
            std::cout << std::endl;
        }

        void getPhenotype(double &cost, uint &gwCount, uint &energy, double &totalUF, bool &feasible) const {
            unsigned int edCount = genes.size();
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (unsigned int i = 0; i < edCount; i++) {
                EdGene* gene = (EdGene*) genes[i];
                gw[i] = gene->getGW();
                sf[i] = gene->getSF();
            }
            // Evaluate cost and objectives
            cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);
        }

        void setGeneValue(uint index, uint gw, uint sf){
            EdGene* gene = (EdGene*) genes[index];
            gene->setValue(gw, sf);
        }

        void printPhenotype() const override {
            double cost;
            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            getPhenotype(cost, gwCount, energy, totalUF, feasible);
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

        void clone(const Chromosome* other) { // Copy the genes from another chromosome
            std::vector<Gene*> otherGenes = other->getGenes();
            std::vector<Gene*> thisGenes = getGenes();
            for (unsigned int i = 0; i < otherGenes.size(); i++) {
                EdGene *thisGene = (EdGene*) thisGenes[i];
                EdGene *otherGene = (EdGene*) otherGenes[i];
                unsigned int gw = otherGene->getGW();
                unsigned int sf = otherGene->getSF();
                thisGene->setValue(gw, sf);
            }
            fitness = other->fitness;
            objectives = other->objectives;
        }
    
    private:
        Objective* o;
};


class GAFitness : public Fitness { // Cost fitness function
    public:
        GAFitness(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
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
            double inverseCost = feasible ? 1e3/cost : 0;
            chromosome->fitness = inverseCost;
            chromosome->objectives = {(double) gwCount, (double) energy, totalUF};
        }

        AllocationChromosome* generateChromosome() const override {
            AllocationChromosome* ch = new AllocationChromosome(o);
            evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};


class GAFitnessGW : public Fitness { // Cost fitness function
    public:
        GAFitnessGW(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
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
            o->eval(gw, sf, gwCount, energy, totalUF, feasible);
            // Inverse cost for maximization
            chromosome->fitness = feasible ? 1e3/gwCount : 0;
            chromosome->objectives = {(double) gwCount, (double) energy, totalUF};
        }

        AllocationChromosome* generateChromosome() const override {
            AllocationChromosome* ch = new AllocationChromosome(o);
            evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};


class GAFitnessE : public Fitness { // Cost fitness function
    public:
        GAFitnessE(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
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
            o->eval(gw, sf, gwCount, energy, totalUF, feasible);
            // Inverse cost for maximization
            chromosome->fitness = feasible ? 1e3/energy : 0;
            chromosome->objectives = {(double) gwCount, (double) energy, totalUF};
        }

        AllocationChromosome* generateChromosome() const override {
            AllocationChromosome* ch = new AllocationChromosome(o);
            evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};

class GAFitnessUF : public Fitness { // Cost fitness function
    public:
        GAFitnessUF(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
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
            o->eval(gw, sf, gwCount, energy, totalUF, feasible);
            // Inverse cost for maximization
            chromosome->fitness = feasible ? 1e3/totalUF : 0;
            chromosome->objectives = {(double) gwCount, (double) energy, totalUF};
        }

        AllocationChromosome* generateChromosome() const override {
            AllocationChromosome* ch = new AllocationChromosome(o);
            evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};