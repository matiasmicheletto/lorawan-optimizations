#include "results.h"

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