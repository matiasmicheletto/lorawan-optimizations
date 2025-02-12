#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <iostream>
#include <vector>
#include <cstdlib>
#include <functional>
#include "gene.h"

using namespace custom_ga;

class Chromosome { // Abstract class that models a chromosome (list of genes with GA operators)
    public:
        Chromosome(){};
        Chromosome(const Chromosome& ch);
        virtual ~Chromosome();

        virtual std::string getName() const = 0;

        inline std::vector<Gene*> getGenes() const { return genes; }
        inline void setGenes(std::vector<Gene*> genes) { this->genes = genes; }

        virtual void printGenotype() const;
        virtual void printPhenotype() const = 0;

        virtual void mutate(); 
        virtual void clone(const Chromosome* other) = 0; // Copy genes and fitness value
        
        virtual void crossover(Chromosome* other);
        void setCrossoverMethod(std::function<void(Chromosome*, Chromosome*)> method);
        virtual void singlePointCrossover(Chromosome* other);
        virtual void doublePointCrossover(Chromosome* other);
        virtual void uniformCrossover(Chromosome* other);

        double fitness; // Fitness value of the chromosome (value is updated by the fitness function)

        // For multi-objective optimization
        std::vector<double> objectives; 
        std::vector<Chromosome*> dominatedChromosomes;
        unsigned int dominationCount;
        double crowdingDistance;
    
    protected:
        Chromosome(double mutProb) : mutProb(mutProb) {}

        std::vector<Gene*> genes; 
        double mutProb;
        custom_ga::Uniform uniform; // NAMESPACE

        std::function<void(Chromosome*, Chromosome*)> crossoverMethod;
};

#endif // CHROMOSOME_H