#ifndef OBJECTIVE_H
#define OBJECTIVE_H

/* 
    Class Objective: Models problem's objective function. Takes a candidate solution
    and returns its objective value. Requires problem model (instance). 
    Optimization variable format:
        -- x[i][0]: GW for ED i.
        -- x[i][1]: SF used by ED i.
*/


#include <algorithm>
#include "../util/util.h"
#include "uf.h"
#include "instance.h"

struct TunningParameters {
    double alpha;
    double beta;
    double gamma;
    TunningParameters( // Default tunning parameters
        double alpha = 1,
        double beta = 0.1,
        double gamma = 7.8
    ) : 
        alpha(alpha),
        beta(beta),
        gamma(gamma) {}
};

struct OptimizationResults {
    bool ready; // Valid content flag
    char* instanceName; // Instance input file name
    char* solverName; // Optimization method used
    double execTime; // Total computing time in ms
    double cost; // Optimal solution cost
    unsigned int gwUsed; // Optimal number of used gw
    unsigned int energy; // Energy of solution
    bool feasible; // Feasible solution
    double uf; // Max. utilization factor of solution
    TunningParameters tp; // alpha, beta, gamma
};

struct EvalResults {
    unsigned int gwUsed; // Optimal number of used gw
    unsigned int energy; // Energy of solution
    bool feasible; // Feasible solution
    double uf; // Max. utilization factor of solution
    double cost; // Objective cost
};

class Objective {
    public:
        Objective(Instance* instance, const TunningParameters& tp = TunningParameters());
        ~Objective();
        
        double eval(const uint* gw, const uint* sf, uint &gwCount, uint &energy, double &totalUF, bool &feasible);
        EvalResults eval(Allocation alloc);

        void printSolution(const uint* gw, const uint* sf, bool allocation = true, bool highlight = false, bool showGWs = false);
        void printSolution(const Allocation alloc, const EvalResults results, bool allocation = true, bool highlight = false, bool showGWs = false);
        void exportWST(const uint* gw, const uint* sf);
        void printSol(const uint* gw, const uint* sf);

        TunningParameters tp;
    private:
        Instance* instance;
        static const uint unfeasibleIncrement;
};

void logResultsToCSV(const OptimizationResults results, const char* csvfilename);

#endif // OBJECTIVE_H