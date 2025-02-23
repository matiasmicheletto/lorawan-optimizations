#include "../custom_ga/uniform.h"
#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"


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

        AllocationChromosome* generateChromosome(CROSS_METHOD cm) const override {
            AllocationChromosome* ch = new AllocationChromosome(o);
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

class GWGAFitnessGW : public Fitness { // Cost fitness function
    public:
        GWGAFitnessGW(Objective* o) : Fitness() {
            this->o = o;
        }

        std::string getName() const override {
            return "Network Fitness";
        }

        void evaluate(Chromosome *chromosome) const override {
            const unsigned int edCount = o->getInstance()->edCount;
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];

            GWAllocationChromosome* ch = (GWAllocationChromosome*) chromosome;

            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            double cost;
            ch->getPhenotype(cost, gwCount, energy, totalUF, feasible);
            chromosome->fitness = feasible ? 1e3/gwCount : 0;
            chromosome->objectives = {(double) gwCount, (double) energy, totalUF};
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