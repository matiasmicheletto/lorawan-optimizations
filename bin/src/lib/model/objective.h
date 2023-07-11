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
#include "uf.h"
#include "instance.h"

struct TunningParameters {
    double alpha;
    double beta;
    double gamma;
    TunningParameters( // Default tunning parameters
        double alpha = 1.0,
        double beta = 0.1,
        double gamma = 1000.0
    ) : 
        alpha(alpha),
        beta(beta),
        gamma(gamma) {}
};


class Objective {
    public:
        Objective(Instance* instance, const TunningParameters& tp = TunningParameters());
        ~Objective();
        
        double eval(unsigned int* gw, unsigned int* sf, unsigned int &gwCount, unsigned int &energy, double &totalUF, bool &feasible);
        TunningParameters tp;
        void printSolution(unsigned int* gw, unsigned int* sf, bool allocation = true, bool highlight = false);
    private:
        Instance* instance;
        static const uint unfeasibleIncrement;
};

#endif // OBJECTIVE_H