#include "objective.h"

Objective::Objective(Instance* instance, const TunningParameters& tp) {
    this->instance = instance;
    this->tp = tp;
}

Objective::~Objective() {

}

const uint Objective::unfeasibleIncrement = 10000;

double Objective::eval(const uint* gw, const uint* sf, uint &gwCount, uint &energy, double &maxUF, bool &feasible) {    
    // Reset objectives (if unfeasible solution, these values remain 0 and _DBL_MAX_ is returned)
    gwCount = 0;
    energy = 0;
    maxUF = 0.0;
    int feasibility = 0; // Feasibility type: 0->feasible, 1 -> no valid SF, 2 -> UF > 1 for some gw
    double cost = 0.0; // Cost value have meaning when solutions are feasible, else will take large values

    UtilizationFactor gwuf[this->instance->gwCount]; // Array of UF objects
    std::fill(gwuf, gwuf + this->instance->gwCount, UtilizationFactor()); // Initialize all elements to 0.0 (all SF from 7 to 12)

    for(uint i = 0; i < this->instance->edCount; i++){ // For each ED    
        // Check if feasible SF
        uint minSF = this->instance->getMinSF(i, gw[i]);
        uint maxSF = this->instance->getMaxSF(i);
        if(sf[i] > maxSF || sf[i] < minSF){ // Unfeasibility condition: not valid SF available for this GW
            feasibility = 1;
            //std::cout << "SF of ED " << i << " not valid." << std::endl;
            cost += unfeasibleIncrement;
        }

        // Compute GW UF
        gwuf[gw[i]] += this->instance->getUF(i, sf[i]);
        if(gwuf[gw[i]].isFull()){ // Unfeasibility condition: UF > 1 for some SF
            feasibility = 2;
            //std::cout << "UF of GW " << gw[i] << " greater than 1" << std::endl;
            //gwuf[gw[i]].printUFValues();
            cost += unfeasibleIncrement;
        }
        double maxUFTemp = gwuf[gw[i]].getMax(); // Max UF value between all SF
        if(maxUFTemp > maxUF) // Update max UF
            maxUF = maxUFTemp;
    }

    // Count number of used gw
    for(uint j = 0; j < this->instance->gwCount; j++)
        if(gwuf[j].isUsed()) 
            gwCount++;

    // Compute energy cost
    for(uint i = 0; i < this->instance->edCount; i++) // For each ED
        energy += this->instance->sf2e(sf[i]);// energy += pow(2, sf[i] - 7);

    feasible = feasibility == 0;

    // If solution is feasible, at this point (before following equation), cost should equal 0.0
    cost += this->tp.alpha * (double) gwCount + 
            this->tp.beta * (double) energy + 
            this->tp.gamma * maxUF;

    return cost; 
}

EvalResults Objective::eval(Allocation alloc) {
    EvalResults res = {
        0, // gwUsed
        0, // energy
        true, // feasible
        0.0, // uf
        0.0 // cost
    };

    for(uint i = 0; i < this->instance->edCount; i++){ // For each ED
        if(alloc.connected[i]){

            // Check if feasible SF
            uint minSF = this->instance->getMinSF(i, alloc.gw[i]);
            uint maxSF = this->instance->getMaxSF(i);
            if(alloc.sf[i] > maxSF || alloc.sf[i] < minSF){ // Unfeasibility condition: not valid SF available for this GW
                res.feasible = false;
                res.cost += unfeasibleIncrement;
            }

            // Compute GW UF
            if(alloc.ufGW[alloc.gw[i]].isFull()){ // Unfeasibility condition: UF > 1 for some SF
                res.feasible = false;
                res.cost += unfeasibleIncrement;
            }

            double maxUFTemp = alloc.ufGW[alloc.gw[i]].getMax(); // Max UF value between all SF
            if(maxUFTemp > res.uf) // Update max UF
                res.uf = maxUFTemp;

            res.energy += this->instance->sf2e(alloc.sf[i]);// energy += pow(2, sf[i] - 7);
        }else{
            res.feasible = false;
            res.cost += 3*unfeasibleIncrement;
        }
    }

    // Count number of used gw
    for(uint j = 0; j < this->instance->gwCount; j++)
        if(alloc.ufGW[j].isUsed())
            res.gwUsed++;

    // If solution is feasible, at this point (before following equation), cost should equal 0.0
    res.cost += this->tp.alpha * (double) res.gwUsed + 
            this->tp.beta * (double) res.energy + 
            this->tp.gamma * res.uf;    

    return res;
}

void Objective::printSolution(const uint* gw, const uint* sf, bool allocation, bool highlight, bool showGWs){
    
    uint gwCount;
    uint energy;
    double maxUF;
    bool feasible;
    const uint edCount = this->instance->edCount;

    const double result = this->eval(gw, sf, gwCount, energy, maxUF, feasible);

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font

    std::cout << "Cost=" << result 
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << maxUF
                << ")" << std::endl;
    
    if(showGWs) {
        std::vector<uint> gwList; // Gws used
        for(uint i = 0; i < edCount; i++) {
            auto it = std::find(gwList.begin(), gwList.end(), gw[i]);
            if(it == gwList.end()){ // If the gw of node "i" isnt in list
                gwList.push_back(gw[i]);
            }
        }
        std::vector<uint> gwList2; // Gws not used
        for (uint gi = 0; gi < this->instance->gwCount; gi++) {
            auto it = std::find(gwList.begin(), gwList.end(), gi);
            if(it == gwList.end()){ // If the gw of node "i" isnt in list
                gwList2.push_back(gi);
            }
        }
        
        std::cout << "GWs used: "; 
        for(uint i = 0; i < gwList.size(); i++)
            std::cout << gwList[i] << " ";
        std::cout << std::endl;
        std::cout << "GWs not used: "; 
        for(uint i = 0; i < gwList2.size(); i++)
            std::cout << gwList2[i] << " ";
        std::cout << std::endl;
    }

    if(allocation){
        std::cout << "Allocation (GW[SF]):" << std::endl;
        for(uint i = 0; i < edCount; i++){ // For each ED    
            if(i % 10 == 0) std::cout << std::endl;
            std::cout << gw[i] << "[" << sf[i] << "]\t";
        }
        std::cout << std::endl;
    }
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
}

void Objective::printSolution(const Allocation alloc, const EvalResults results, bool allocation, bool highlight, bool showGWs) {

    if(highlight) std::cout << "\033[1;31m"; // Switch to red font
    
    std::cout << "Cost=" << results.cost << (results.feasible ? " (Feasible)" : " (Unfeasible)");

    if(results.feasible){
        std::cout << ", (GW=" << results.gwUsed
                  << ",E=" << results.energy 
                  << ",U=" << results.uf
                  << ")" << std::endl;

        if(showGWs) {
            std::vector<uint> gwList; // Used
            std::vector<uint> gwList2; // Unused
            for(uint i = 0; i < alloc.gw.size(); i++){
                auto it = std::find(gwList.begin(), gwList.end(), alloc.gw[i]);
                if(it == gwList.end()){ // If the gw of node "i" isnt in list
                    gwList.push_back(alloc.gw[i]);
                }
            }
            for (uint gi = 0; gi < this->instance->gwCount; gi++) {
                auto it = std::find(gwList.begin(), gwList.end(), gi);
                if(it == gwList.end()){ // If the gw of node "i" isnt in list
                    gwList2.push_back(gi);
                }
            }
            std::cout << "Indexes of GWs used: "; 
            for(uint i = 0; i < gwList.size(); i++)
                std::cout << gwList[i] << " ";
            std::cout << std::endl;
            std::cout << "Indexes of GWs not used: "; 
            for(uint i = 0; i < gwList2.size(); i++)
                std::cout << gwList2[i] << " ";
            std::cout << std::endl;
        }

        if(allocation){
            std::cout << "Allocation (GW[SF]):" << std::endl;
            for(uint i = 0; i < alloc.gw.size(); i++){ // For each ED    
                if(i % 10 == 0) std::cout << std::endl;
                std::cout << alloc.gw[i] << "[" << alloc.sf[i] << "]\t";
            }
            std::cout << std::endl;
        }
    }
    
    if(highlight) std::cout << "\033[0m\n"; // Switch to normal text font
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
    for(uint g = 1; g < this->instance->gwCount+1; g++){
        selected = false;
        for(uint e = 0; e < this->instance->edCount; e++){
            if(gw[e] == g-1){
                selected = true;
                break;
            }
        }
        for(int ch = 15; ch >= 0; ch--){
            std::cout << "    <variable name=\"w#" << g << "#" << ch 
                    << "\" index=\"" << index
                    << "\" value=\"" << (selected && (uint)ch == lastch ? 1 : 0)
                    << "\"/>" << std::endl;
            index++;
        }
        if(selected) lastch++;
        if(lastch == 16) lastch = 0;
    }

    for(uint e = 1; e < this->instance->edCount+1; e++)
        for(uint g = this->instance->gwCount; g > 0 ; g--)   
            for(uint s = 12; s >= 7; s--){
                selected = gw[e-1] == g-1 && sf[e-1] == s;
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

void logResultsToCSV(const OptimizationResults results, const char* csvfilename) {
    std::ofstream csvFile(csvfilename, std::ios::app); // Open file in append mode

    if (!csvFile) {
        std::cerr << "Error: Unable to open CSV file." << std::endl;
        return;
    }

    if (csvFile.tellp() == 0) { // File is empty, write the header
        csvFile << "Instance Name,Alpha,Beta,Gamma,Solver,Execution Time (ms),Cost,Feasible,GW Used,Energy,UF" << std::endl;
    }
    
    csvFile << results.instanceName << ","
            << results.tp.alpha << ","
            << results.tp.beta << ","
            << results.tp.gamma << ","
            << results.solverName << ","
            << results.execTime << ","
            << results.cost << ","
            << (results.feasible ? "Yes,":"No,")
            << results.gwUsed << ","
            << results.energy << ","
            << results.uf << std::endl;

    csvFile.flush();
    csvFile.close();
}
