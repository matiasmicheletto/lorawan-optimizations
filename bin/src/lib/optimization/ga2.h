#include "../custom_ga/uniform.h"
#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"


using namespace custom_ga;

struct EdSf { // End device and SF
    unsigned int ed; // ED index
    unsigned int sf; // Values from 7 to 12
};


class GWAllocationChromosome : public Chromosome { // Network allocation (GW and SF for each ED)
    public:
        GWAllocationChromosome(Objective* o) : Chromosome(o->getInstance()->edCount) {
            this->o = o;
            Instance *l = o->getInstance();
            for(uint ed = 0; ed < l->edCount; ed++) {
                std::vector<uint> gwIndexes = l->getGWList(ed); // Valid gws for this ed
                // Choose a random gw from the list
                const unsigned int gwIndex = uniform.random(gwList.size());
                const uint minSF = l->getMinSF(ed, gwIndex);
                const uint maxSF = l->getMaxSF(ed);
                // Put the ed in the gw
                EdSf edSf = {ed, uniform.random(minSF, maxSF)};
                gwList[gwIndex].push_back(edSf);
            }
        }

        std::string getName() const override {
            return "Allocation array";
        }

        void printGenotype() const override {
            std::cout << "Genotype: ";
            for (unsigned int i = 0; i < gwList.size(); i++) {
                std::cout << "GW " << i << ": ";
                for (EdSf edSf : gwList[i]) {
                    std::cout << edSf.ed << "[" << edSf.sf << "] ";
                }
                std::cout << std::endl;
            }    
        }

        void getPhenotype(double &cost, uint &gwCount, uint &energy, double &totalUF, bool &feasible) const {
            // Evaluate cost and objectives
            unsigned int edCount = o->getInstance()->edCount;
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (unsigned int i = 0; i < edCount; i++) {
                gw[i] = -1;
                sf[i] = -1;
            }
            for (unsigned int i = 0; i < gwList.size(); i++) {
                for (EdSf edSf : gwList[i]) {
                    gw[edSf.ed] = i;
                    sf[edSf.ed] = edSf.sf;
                }
            }
            cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);
        }

        void pushElToGene(unsigned int gw, EdSf edSf) { // Add an ED to a GW
            if (gw <= gwList.size()) {
                gwList[gw].push_back(edSf);   
            }
        }

        void printPhenotype() const override {
            double cost;
            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            getPhenotype(cost, gwCount, energy, totalUF, feasible);
            std::cout << "Cost=" << cost
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << totalUF
                << ")" << std::endl;
        }

        void mutate() override {
            // Sort list of GW from least to most number of EDs
            std::sort(gwList.begin(), gwList.end(), [](std::vector<EdSf> a, std::vector<EdSf> b) {
                return a.size() < b.size();
            });
            // Move one ed from the GW with the least EDs to the one with the most
            unsigned int minGW = 0;
            unsigned int maxGW = gwList.size() - 1;
            unsigned int minED = 0;
            unsigned int maxED = gwList[minGW].size() - 1;
            unsigned int ed = rand() % gwList[minGW].size();
            EdSf edSf = gwList[minGW][ed];
            gwList[minGW].erase(gwList[minGW].begin() + ed);
            gwList[maxGW].push_back(edSf);
        }

        void singlePointCrossover(Chromosome* other) override {
            GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            unsigned int pivot = rand() % gwList.size();
            for (unsigned int i = pivot; i < gwList.size(); i++) {
                std::vector<EdSf> aux = gwList[i];
                gwList[i] = ot->getGeneList()[i];
                ot->getGeneList()[i] = aux;
            }   
        }

        void doublePointCrossover(Chromosome* other) override {
            GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            unsigned int point1 = rand() % gwList.size();
            unsigned int point2 = rand() % gwList.size();
            if (point1 > point2) {
                unsigned int aux = point1;
                point1 = point2;
                point2 = aux;
            }
            for (unsigned int i = point1; i < point2; i++) {
                std::vector<EdSf> aux = gwList[i];
                gwList[i] = ot->getGeneList()[i];
                ot->getGeneList()[i] = aux;
            }
        }

        void uniformCrossover(Chromosome* other) override {
            GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            for (unsigned int i = 0; i < gwList.size(); i++) {
                if (rand() % 2 == 0) {
                    std::vector<EdSf> aux = gwList[i];
                    gwList[i] = ot->getGeneList()[i];
                    ot->getGeneList()[i] = aux;
                }
            }
        }

        void clone(const Chromosome* other) { // Copy the genes from another chromosome
            const GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            this->gwList = ot->getGeneList();
            fitness = other->fitness;
            objectives = other->objectives;
        }

        std::vector<std::vector<EdSf>> getGeneList() const {
            return gwList;
        }
    
    private:
        Objective* o;
        std::vector<std::vector<EdSf>> gwList; // List of EDs assigned to each GW
};


class GWGAFitness : public Fitness { // Cost fitness function
    public:
        GWGAFitness(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
            
        }

        GWAllocationChromosome* generateChromosome(CROSS_METHOD cm) const override {
            GWAllocationChromosome* ch = new GWAllocationChromosome(o);
            switch (cm) {
                case CROSS_METHOD::SINGLE_POINT:
                    ch->setCrossoverMethod(&Chromosome::singlePointCrossover);
                    break;
                case CROSS_METHOD::DOUBLE_POINT:
                    ch->setCrossoverMethod(&Chromosome::doublePointCrossover);
                    break;
                case CROSS_METHOD::C_UNIFORM:
                    ch->setCrossoverMethod(&Chromosome::uniformCrossover);
                    break;
            }
            evaluate(ch);
            return ch;
        }

    private:
        Objective* o;
};
