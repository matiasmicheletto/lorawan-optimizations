#ifndef CUSTOM_RANDOM_H
#define CUSTOM_RANDOM_H

#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

double uniform() {
    return dis(gen);
}

double uniform(double to){
    double value = dis(gen)*to;
    return value;
}

double uniform(double from, double to){
    double value = from + dis(gen)*(to-from);
    return value;
}

#endif