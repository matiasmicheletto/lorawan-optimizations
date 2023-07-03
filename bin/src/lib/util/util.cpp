#include "util.h"

void printHelp(const char* file) {    
    std::ifstream manualFile(file);
    if (manualFile.is_open()) {
        std::string line;
        while (getline(manualFile, line)) {
            std::cout << line << std::endl;
        }
        manualFile.close();
        exit(1);
    } else {
        std::cerr << "Error: Unable to open manual file." << std::endl;
    }
}

double euclideanDistance(double x1, double y1, double x2, double y2) {
    double discr = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
    return sqrt(discr);
}

void removeRowsAndColumn(std::vector<std::vector<unsigned int>>& matrix, unsigned int columnIndex, const std::vector<unsigned int>& rowIndices) {
    for (auto it = rowIndices.rbegin(); it != rowIndices.rend(); ++it)
        if (*it < matrix.size())
            matrix.erase(matrix.begin() + *it);

    for (auto& row : matrix)
        if (columnIndex < row.size())
            row.erase(row.begin() + columnIndex);
}

void copyMatrix(const std::vector<std::vector<unsigned int>>& source, std::vector<std::vector<unsigned int>>&destination) {
    destination.reserve(source.size());
    for (const auto& row : source)
        destination.push_back(row);
}

void copyMatrix(const std::vector<std::vector<unsigned int>>& source, std::vector<std::vector<unsigned int>>& destination, unsigned int startRow, unsigned int endRow, unsigned int startCol, unsigned int endCol) {
    unsigned int numRows = endRow - startRow + 1;
    unsigned int numCols = endCol - startCol + 1;

    destination.resize(numRows);
    for (int i = 0; i < numRows; i++) {
        destination[i].resize(numCols);
        for (int j = 0; j < numCols; j++) {
            destination[i][j] = source[startRow + i][startCol + j];
        }
    }
}