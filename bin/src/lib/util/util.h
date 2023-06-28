#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>


void printHelp(const char* file);
unsigned int sf2e(unsigned int sf);
double euclideanDistance(double x1, double y1, double x2, double y2);
template <typename T>
void removeRowsAndColumn(std::vector<std::vector<T>>& matrix, size_t columnIndex, const std::vector<size_t>& rowIndices);
template <typename T>
std::vector<std::vector<T>> copyMatrix(const std::vector<std::vector<T>>& matrix);

#endif // UTIL_H