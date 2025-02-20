#include "../custom_ga/uniform.h"
#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"


using namespace custom_ga;

class GWAllocationChromosome : public Chromosome { // Network allocation (GW and SF for each ED)
    public:
        GWAllocationChromosome(Objective* o) : Chromosome(o->getInstance()->edCount) {
            this->o = o;
            
        }

        std::string getName() const override {
            return "Allocation array";
        }

        void printGenotype() const override {
            std::cout << "Genotype: ";
        }

        void getPhenotype(double &cost, uint &gwCount, uint &energy, double &totalUF, bool &feasible) const {
            // Evaluate cost and objectives
            //cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);
        }

        void setGeneValue(uint index, uint gw, uint sf){
            
        }

        void printPhenotype() const override {
            
        }

        void clone(const Chromosome* other) { // Copy the genes from another chromosome
            
        }
    
    private:
        Objective* o;
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
