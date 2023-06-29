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
#include "../instance/instance.h"

// UF value for a GW to be considered as "used"
#define UFTHRES .0000001

struct TunningParameters {
    double alpha;
    double beta;
    double gamma;
    TunningParameters( // Default tunning parameters
        double alpha = 1.0,
        double beta = 1.0,
        double gamma = 1.0
    ) : 
        alpha(alpha),
        beta(beta),
        gamma(gamma) {}
};


class Objective {
    public:
        Objective(Instance* instance, const TunningParameters& tp = TunningParameters());
        ~Objective();
        
        double eval(unsigned int* gw, unsigned int* sf, unsigned int &gwCount, unsigned int &energy, double &totalUF);
        TunningParameters tp;
        void printSolution(unsigned int* gw, unsigned int* sf, bool highlight = false);
    private:
        Instance* instance;
};

#endif // OBJECTIVE_H