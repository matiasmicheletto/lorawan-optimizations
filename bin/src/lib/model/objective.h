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

enum FEAS_CODE {FEASIBLE, SF_RANGE, UF_VALUE, ED_COVERAGE};

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
    uint gwUsed; // Optimal number of used gw
    uint energy; // Energy of solution
    bool feasible; // Feasible solution
    double uf; // Max. utilization factor of solution
    TunningParameters tp; // alpha, beta, gamma

    void print(int detailLevel = 0) {
        switch (detailLevel)
        {
            case 0:
                std::cout << "Cost=" << cost << (feasible ? " (Feasible)" : " (Unfeasible)");
                if(feasible){
                    std::cout << ", (GW=" << gwUsed
                            << ",E=" << energy 
                            << ",U=" << uf
                            << ")" << std::endl;
                    std::cout << "Total execution time = " << execTime << " ms" << std::endl;
                }
                break;
            case 1:
                std::cout << "Results:" << std::endl;
                std::cout << "  Instance: " << instanceName << std::endl;
                std::cout << "  Solver: " << solverName << std::endl;
                std::cout << "  Execution time: " << execTime << " ms" << std::endl;
                std::cout << "  Cost: " << cost << std::endl;
                std::cout << "  GW used: " << gwUsed << std::endl;
                std::cout << "  Energy: " << energy << std::endl;
                std::cout << "  Feasible: " << (feasible ? "Yes" : "No") << std::endl;
                std::cout << "  Max UF: " << uf << std::endl;
                std::cout << "  Tunning parameters:" << std::endl;
                std::cout << "    Alpha: " << tp.alpha << std::endl;
                std::cout << "    Beta: " << tp.beta << std::endl;
                std::cout << "    Gamma: " << tp.gamma << std::endl;
                break;
            default:
                std::cerr << "Result printing error: Unknown detail level." << std::endl;
                break;
        }
    }
};

struct EvalResults {
    uint gwUsed; // Optimal number of used gw
    uint energy; // Energy of solution
    bool feasible; // Feasible solution
    FEAS_CODE unfeasibleCode; // Unfeasibility condition
    double uf; // Max. utilization factor of solution
    double cost; // Objective cost
};

class Objective {
    public:
        Objective(Instance* instance, const TunningParameters& tp = TunningParameters());
        ~Objective();
        
        double eval(const uint* gw, const uint* sf, uint &gwCount, uint &energy, double &totalUF, bool &feasible);
        EvalResults eval(Allocation alloc, bool comparable = false);

        void printSolution(const uint* gw, const uint* sf, bool allocation = true, bool highlight = false, bool showGWs = false, std::ostream& os = std::cout);
        void printSolution(const Allocation alloc, const EvalResults results, bool allocation = true, bool highlight = false, bool showGWs = false, std::ostream& os = std::cout);
        void exportWST(const uint* gw, const uint* sf, std::ostream& os = std::cout);

        TunningParameters tp;

        inline Instance *getInstance() const { return instance; }
    private:
        Instance* instance;
        static const uint unfeasibleIncrement;
};

void logResultsToCSV(const OptimizationResults results, const char* csvfilename);

#endif // OBJECTIVE_H