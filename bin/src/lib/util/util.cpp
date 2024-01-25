#include "util.h"

void printHelp(const char* file) { // Open readme file with manual and print on terminal   
    std::ifstream manualFile(file);
    if (manualFile.is_open()) {
        std::string line;
        while (getline(manualFile, line)) {
            std::cout << line << std::endl;
        }
        manualFile.close();
    } else {
        std::cerr << "Error: Unable to open manual file." << std::endl;
    }
    exit(1);
}

char* extractFileName(const char* path) { // Remove path from full file name
    const char* fileNameStart = strrchr(path, '/'); // Find the last occurrence of '/'
    if (fileNameStart == nullptr) {
        fileNameStart = strrchr(path, '\\'); // If '/' is not found, try '\' for Windows paths
        if (fileNameStart == nullptr) {
            fileNameStart = path; // If neither '/' nor '\' is found, assume the whole path is the file name
        } else {
            fileNameStart++; // Move past the '\' character
        }
    } else {
        fileNameStart++; // Move past the '/' character
    }

    // Create a new string to hold the extracted file name
    char* fileName = new char[strlen(fileNameStart) + 1];
    strcpy(fileName, fileNameStart); // Copy the file name to the new string

    return fileName;
}

double euclideanDistance(double x1, double y1, double x2, double y2) {
    double discr = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
    return sqrt(discr);
}

void removeRowsAndColumn(std::vector<std::vector<uint>>& matrix, uint columnIndex, const std::vector<uint>& rowIndices) {
    for (auto it = rowIndices.rbegin(); it != rowIndices.rend(); ++it)
        if (*it < matrix.size())
            matrix.erase(matrix.begin() + *it);

    for (auto& row : matrix)
        if (columnIndex < row.size())
            row.erase(row.begin() + columnIndex);
}

void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>&destination) {
    destination.reserve(source.size());
    for (const auto& row : source)
        destination.push_back(row);
}

void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>& destination, uint startRow, uint endRow, uint startCol, uint endCol) {
    uint numRows = endRow - startRow + 1;
    uint numCols = endCol - startCol + 1;

    destination.resize(numRows);
    for (uint i = 0; i < numRows; i++) {
        destination[i].resize(numCols);
        for (uint j = 0; j < numCols; j++) {
            destination[i][j] = source[startRow + i][startCol + j];
        }
    }
}