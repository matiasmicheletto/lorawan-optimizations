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

template <typename T>
void removeRowsAndColumn(std::vector<std::vector<T>>& matrix, size_t columnIndex, const std::vector<size_t>& rowIndices) {
    for (auto it = rowIndices.rbegin(); it != rowIndices.rend(); ++it)
        if (*it < matrix.size())
            matrix.erase(matrix.begin() + *it);

    for (auto& row : matrix)
        if (columnIndex < row.size())
            row.erase(row.begin() + columnIndex);
}

template <typename T>
std::vector<std::vector<T>> copyMatrix(const std::vector<std::vector<T>>& matrix) {
    std::vector<std::vector<T>> copy;
    copy.reserve(matrix.size());

    for (const auto& row : matrix)
        copy.push_back(row);

    return copy;
}

unsigned int sf2e(unsigned int sf){
    static const unsigned int arr[6] = {1, 2, 4, 8, 16, 32};
    return arr[sf-7];
}