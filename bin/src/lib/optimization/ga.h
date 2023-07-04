#ifndef GA_H
#define GA_H

#include "../instance/instance.h"
#include "../objective/objective.h"
#include "results.h"


struct GAConfig {
    int popsize;
    int maxgen;
    double crossover;
    double mutation;
    
    GAConfig(
            int popSize = 40, 
            int numGens = 1000, 
            double crossRate = 0.8, 
            double mutRate = 0.3)
        : 
            popsize(popSize), 
            maxgen(numGens), 
            crossover(crossRate), 
            mutation(mutRate) {}
};

OptimizationResults ga(Instance* l, Objective* o, const GAConfig& config = GAConfig(), bool verbose = false);

#endif // GA_H