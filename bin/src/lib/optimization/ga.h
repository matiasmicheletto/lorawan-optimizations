#ifndef GA_H
#define GA_H

#include "../model/instance.h"
#include "../model/objective.h"
#include "results.h"


struct GAConfig {
    int popsize;
    int maxgen;
    double crossover;
    double mutation;
    
    GAConfig(
            int popSize = 50, 
            int numGens = 1000, 
            double crossRate = 0.7, 
            double mutRate = 0.5)
        : 
            popsize(popSize), 
            maxgen(numGens), 
            crossover(crossRate), 
            mutation(mutRate) {}
};

OptimizationResults ga(Instance* l, Objective* o, const GAConfig& config = GAConfig(), bool verbose = false);
OptimizationResults nsga(Instance* l, Objective* o, const GAConfig& config = GAConfig(), bool verbose = false);

#endif // GA_H