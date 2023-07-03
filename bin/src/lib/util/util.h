#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>

using uint = unsigned int;

void printHelp(const char* file);
double euclideanDistance(double x1, double y1, double x2, double y2);
void removeRowsAndColumn(std::vector<std::vector<uint>>& matrix, uint columnIndex, const std::vector<uint>& rowIndices);
void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>& destination);
void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>& destination, uint startRow, uint endRow, uint startCol, uint endCol);

#endif // UTIL_H