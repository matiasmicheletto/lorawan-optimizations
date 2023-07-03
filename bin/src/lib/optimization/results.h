#ifndef RESULTS_H
#define RESULTS_H

#include <fstream>
#include "../objective/objective.h"

struct OptimizationResults {
    bool withResults; // Valid content flag
    char* instanceName; // Instance input file name
    char* solver; // Optimization method used (name)
    double execTime; // Total computing time in ms
    double cost; // Optimal solution cost
    unsigned int gwUsed; // Optimal number of used gw
    unsigned int energy; // Energy of solution
    double uf; // Max. utilization factor of solution
    TunningParameters tp; // alpha, beta, gamma
};

void logResultsToCSV(OptimizationResults results, const char* csvfilename);

#endif // RESULTS_H