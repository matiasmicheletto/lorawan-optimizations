#include "results.h"

void logResultsToCSV(const OptimizationResults results, const char* csvfilename) {
    std::ofstream csvFile(csvfilename, std::ios::app); // Open file in append mode

    if (!csvFile) {
        std::cerr << "Error: Unable to open CSV file." << std::endl;
        return;
    }

    if (csvFile.tellp() == 0) { // File is empty, write the header
        csvFile << "Instance Name,Solver,Execution Time (ms),Cost,GW Used,Energy,UF,Alpha,Beta,Gamma" << std::endl;
    }
    
    csvFile << results.instanceName << ","
            << results.solverName << ","
            << results.execTime << ","
            << results.cost << ","
            << results.gwUsed << ","
            << results.energy << ","
            << results.uf << ","
            << results.tp.alpha << ","
            << results.tp.beta << ","
            << results.tp.gamma << std::endl;

    csvFile.flush();
    csvFile.close();

    free(results.solverName);
    //free(results.instanceName); // This pointer is freed in instance destructor
}