#ifndef RESULTS_H
#define RESULTS_H

#include <fstream>
#include "../objective/objective.h"

struct OptimizationResults {
    bool ready; // Valid content flag
    char* instanceName; // Instance input file name
    char* solverName; // Optimization method used
    double execTime; // Total computing time in ms
    double cost; // Optimal solution cost
    unsigned int gwUsed; // Optimal number of used gw
    unsigned int energy; // Energy of solution
    double uf; // Max. utilization factor of solution
    TunningParameters tp; // alpha, beta, gamma
};

void logResultsToCSV(const OptimizationResults results, const char* csvfilename);

#endif // RESULTS_H