#include "results.h"

void logResultsToCSV(OptimizationResults results, const char* csvfilename) {
    std::ofstream csvFile;
    csvFile.open(csvfilename, std::ios::app); // Open append mode
    bool fileExists = csvFile.is_open();

    if (!csvFile.is_open()) 
        csvFile.open(csvfilename); // Open in write mode (first time)

    if (!csvFile.is_open()) { // Failed to open
        std::cerr << "Error: Unable to open CSV file." << std::endl;
        return;
    }

    if (!fileExists) 
        csvFile << "Instance Name,Method,Execution Time (ms),Cost,GW,Energy,UF,Alpha,Beta,Gamma" << std::endl;

    // Write the values to the CSV file
    csvFile << results.instanceName << ","
            << results.solver << ","
            << results.execTime << ","
            << results.cost << ","
            << results.gwUsed << ","
            << results.energy << ","
            << results.uf << ","
            << results.tp.alpha << ","
            << results.tp.beta << ","
            << results.tp.gamma << std::endl;

    // Close the file
    csvFile.close();
}