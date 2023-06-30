#include "objective.h"

Objective::Objective(Instance* instance, const TunningParameters& tp) {
    this->instance = instance;
    this->tp = tp;
}

Objective::~Objective() {

}

double Objective::eval(unsigned int* gw, unsigned int* sf, unsigned int &gwCount, unsigned int &energy, double &maxUF) {    
    // Reset objectives (if unfeasible solution, these values remain 0 and _DBL_MAX_ is returned)
    gwCount = 0;
    energy = 0;
    maxUF = 0.0;

    double gwuf[this->instance->getGWCount()];
    std::fill(gwuf, gwuf + this->instance->getGWCount(), 0.0); // Initialize all elements to 0.0

    for(unsigned int i = 0; i < this->instance->getEDCount(); i++){ // For each ED    
        // Check if feasible SF
        unsigned int minSF = this->instance->getMinSF(i, gw[i]);
        unsigned int maxSF = this->instance->getMaxSF(i);
        if(sf[i] > maxSF || sf[i] < minSF) // Unfeasibility condition: not valid SF
            return __DBL_MAX__;

        // Compute GW UF
        gwuf[gw[i]] += this->instance->getUF(i, sf[i]);
        if(gwuf[gw[i]] > 1.0) // Unfeasibility condition: UF > 1
            return __DBL_MAX__;
        if(gwuf[gw[i]] > maxUF) // Update max u
            maxUF = gwuf[gw[i]];
    }

    // Count number of used gw
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(gwuf[j] > UFTHRES) gwCount++;

    // Compute energy cost
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += this->instance->sf2e(sf[i]);// energy += pow(2, sf[i] - 7);

    return // Return weighted sum of objectives
        this->tp.alpha * (double)gwCount + 
        this->tp.beta * (double) energy + 
        this->tp.gamma * maxUF;
}

void Objective::printSolution(unsigned int* gw, unsigned int* sf, bool highlight){
    unsigned int gwCount;
    unsigned int energy;
    double maxUF;

    const double result = this->eval(gw, sf, gwCount, energy, maxUF);

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font

    std::cout << "Cost=" << result 
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << maxUF
                << ")" << std::endl;
    std::cout << "GW allocation (GW[SF]):" << std::endl;
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED    
        std::cout << gw[i] << "[" << sf[i] << "]  ";
    std::cout << std::endl;
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
}
