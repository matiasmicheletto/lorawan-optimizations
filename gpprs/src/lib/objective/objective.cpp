#include "objective.h"

Objective::Objective(Instance* instance) {
    this->instance = instance;

    // Tunning parameters
    this->params[ALPHA] = 1.0;
    this->params[BETA] = 1.0;
    this->params[GAMMA] = 1.0;
}

Objective::~Objective() {

}

double Objective::eval(unsigned int* gw, unsigned int* sf, unsigned int &gwCount, unsigned int &energy, double &totalUF) {    
    // Reset objectives (if unfeasible solution, these values remain 0 and _DBL_MAX_ is returned)
    gwCount = 0;
    energy = 0;
    totalUF = 0.0;

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
        totalUF += gwuf[gw[i]];
        if(gwuf[gw[i]] > 1.0) // Unfeasibility condition: UF > 1
            return __DBL_MAX__;
    }

    // Count number of used gw
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(gwuf[j] > UFTHRES) gwCount++;

    // Compute energy cost
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += sf2e(sf[i]);// energy += pow(2, sf[i] - 7);

    return // Return weighted sum of objectives
        this->params[ALPHA] * (double)gwCount + 
        this->params[BETA] * (double) energy + 
        this->params[GAMMA] * totalUF;
}

void Objective::printSolution(unsigned int* gw, unsigned int* sf, bool highlight){
    unsigned int gwCount;
    unsigned int energy;
    double totalUF;

    const double result = this->eval(gw, sf, gwCount, energy, totalUF);

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font

    std::cout << "Cost=" << result 
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",UF=" << totalUF
                << ")" << std::endl;
    std::cout << "GW allocation (GW[SF]):" << std::endl;
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED    
        std::cout << gw[i] << "[" << sf[i] << "]  ";
    std::cout << std::endl;
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
}
