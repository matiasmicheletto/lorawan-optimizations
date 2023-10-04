#include "objective.h"

Objective::Objective(Instance* instance, const TunningParameters& tp) {
    this->instance = instance;
    this->tp = tp;
}

Objective::~Objective() {

}

const uint Objective::unfeasibleIncrement = 1000000;

double Objective::eval(const uint* gw, const uint* sf, uint &gwCount, uint &energy, double &maxUF, bool &feasible) {    
    // Reset objectives (if unfeasible solution, these values remain 0 and _DBL_MAX_ is returned)
    gwCount = 0;
    energy = 0;
    maxUF = 0.0;
    feasible = true;
    double cost = 0.0; // Cost value have meaning when solutions are feasible, else will take large values

    UtilizationFactor gwuf[this->instance->getGWCount()]; // Array of UF objects
    std::fill(gwuf, gwuf + this->instance->getGWCount(), UtilizationFactor()); // Initialize all elements to 0.0 (all SF from 7 to 12)

    for(uint i = 0; i < this->instance->getEDCount(); i++){ // For each ED    
        // Check if feasible SF
        uint minSF = this->instance->getMinSF(i, gw[i]);
        uint maxSF = this->instance->getMaxSF(i);
        if(sf[i] > maxSF || sf[i] < minSF){ // Unfeasibility condition: not valid SF available for this GW
            feasible = false;
            cost += unfeasibleIncrement;
        }

        // Compute GW UF
        gwuf[gw[i]] += this->instance->getUF(i, sf[i]);
        if(gwuf[gw[i]].isFull()){ // Unfeasibility condition: UF > 1 for some SF
            feasible = false;
            cost += unfeasibleIncrement;
        }
        double maxUFTemp = gwuf[gw[i]].getMax(); // Max UF value between all SF
        if(maxUFTemp > maxUF) // Update max UF
            maxUF = maxUFTemp;
    }

    // Count number of used gw
    for(uint j = 0; j < this->instance->getGWCount(); j++)
        if(gwuf[j].isUsed()) 
            gwCount++;

    // Compute energy cost
    for(uint i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += this->instance->sf2e(sf[i]);// energy += pow(2, sf[i] - 7);

    // If solution is feasible, at this point, cost should equal 0.0
    cost += this->tp.alpha * (double) gwCount + 
            this->tp.beta * (double) energy + 
            this->tp.gamma * maxUF;

    return cost; 
}

void Objective::printSolution(const uint* gw, const uint* sf, bool allocation, bool highlight){
    
    uint gwCount;
    uint energy;
    double maxUF;
    bool feasible;

    const double result = this->eval(gw, sf, gwCount, energy, maxUF, feasible);

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font

    std::cout << "Cost=" << result 
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << maxUF
                << ")" << std::endl;
    
    if(allocation){
        std::cout << "GW allocation (GW[SF]):" << std::endl;
        for(uint i = 0; i < this->instance->getEDCount(); i++) // For each ED    
            std::cout << gw[i] << "[" << sf[i] << "]  ";
        std::cout << std::endl;
        //this->printSol(gw, sf); // Export file
    }
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
}

void Objective::printSol(const uint* gw, const uint* sf) {
    std::ofstream outFile("last.out", std::ios::app); 

    if (!outFile) {
        std::cerr << "Error: Unable to open CSV file." << std::endl;
        return;
    }
    
    for(uint i = 0; i < this->instance->getEDCount(); i++) // For each ED    
        outFile << i+1 << "\t" << gw[i]+1 << "\t" << sf[i] << std::endl;
    
    outFile.flush();
    outFile.close();
}

void Objective::exportWST(const uint* gw, const uint* sf) {
    std::cout << "<?xml version = \"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl
            << "<CPLEXSolutions version=\"1.2\">" << std::endl
            << "  <CPLEXSolution version=\"1.2\">" << std::endl
            << "   <header" << std::endl
            << "    problemName=\"proof.lp\"" << std::endl
            << "    solutionName=\"m1\"" << std::endl
            << "    solutionIndex=\"0\"" << std::endl
            << "    MIPStartEffortLevel=\"0\"" << std::endl
            << "    writeLevel=\"2\"/>" << std::endl
            << "  <variables>" << std::endl;

    uint index = 3;
    uint lastch = 0;
    bool selected;
    for(uint g = 1; g < this->instance->getGWCount()+1; g++){
        selected = false;
        for(uint e = 0; e < this->instance->getEDCount(); e++){
            if(gw[e] == g){
                selected = true;
                break;
            }
        }
        for(int ch = 15; ch >= 0; ch--){
            std::cout << "    <variable name=\"w#" << g << "#" << ch 
                    << "\" index=\"" << index
                    << "\" value=\"" << (selected && ch==lastch ? 1 : 0)
                    << "\"/>" << std::endl;
            index++;
        }
        if(selected) lastch++;
        if(lastch == 16) lastch = 0;
    }

    for(uint e = 1; e < this->instance->getEDCount()+1; e++)
        for(uint g = this->instance->getGWCount(); g > 0 ; g--)   
            for(uint s = 12; s >= 7; s--){
                selected = gw[e-1] == g && sf[e-1] == s;
                std::cout << "    <variable name=\"x#" 
                        << e << "#" << g << "#" << s
                        << "\" index=\"" << index
                        << "\" value=\"" << (selected ? 1 : 0)
                        << "\"/>" << std::endl;
                index++;
            }

    std::cout << "  </variables>" << std::endl
            << " </CPLEXSolution>" << std::endl
            << "</CPLEXSolutions>" << std::endl;
}