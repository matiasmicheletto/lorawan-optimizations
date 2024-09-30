#include "uniform.h"

Uniform2::Uniform2() {
    gen = std::mt19937(rd());
    dis = std::uniform_real_distribution<>(0.0, 1.0);
}

Uniform2::~Uniform2() {}

double Uniform2::random() {
    return dis(gen);
}

double Uniform2::random(double to) {
    double value = dis(gen) * to;
    return value;
}

double Uniform2::random(double from, double to) {
    double value = from + dis(gen) * (to - from);
    return value;
}