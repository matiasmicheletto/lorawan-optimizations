#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>


void printHelp(const char* file);
double euclideanDistance(double x1, double y1, double x2, double y2);
void removeRowsAndColumn(std::vector<std::vector<unsigned int>>& matrix, unsigned int columnIndex, const std::vector<unsigned int>& rowIndices);
void copyMatrix(const std::vector<std::vector<unsigned int>>& source, std::vector<std::vector<unsigned int>>& destination);
void copyMatrix(const std::vector<std::vector<unsigned int>>& source, std::vector<std::vector<unsigned int>>& destination, unsigned int startRow, unsigned int endRow, unsigned int startCol, unsigned int endCol);

#endif // UTIL_H