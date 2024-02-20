#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <math.h>

//using uint = unsigned int; // Just for shorten the code
#define uint size_t

void printHelp(const char* file);
char* extractFileName(const char* path);
double euclideanDistance(double x1, double y1, double x2, double y2);
void removeRowsAndColumn(std::vector<std::vector<uint>>& matrix, uint columnIndex, const std::vector<uint>& rowIndices);
void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>& destination);
void copyMatrix(const std::vector<std::vector<uint>>& source, std::vector<std::vector<uint>>& destination, uint startRow, uint endRow, uint startCol, uint endCol);

#endif // UTIL_H