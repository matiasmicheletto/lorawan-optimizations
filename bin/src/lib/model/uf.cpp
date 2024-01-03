#include "uf.h"

// Default constructor
UtilizationFactor::UtilizationFactor() {
    for (uint i = 0; i < SF_NUM; i++)
        this->ufValues[i] = 0.0;
}

// Constructor with single SF value
UtilizationFactor::UtilizationFactor(uint sf, double ufValue) {
    if (sf-7 < SF_NUM){ 
        for (uint i = 0; i < SF_NUM; i++)
            this->ufValues[i] = (i == (sf-7) ? ufValue : 0.0);
    }else{
        std::cerr << "Invalid SF index. SF = " << sf << std::endl;
    }
}

// Constructor with initializer list
UtilizationFactor::UtilizationFactor(std::initializer_list<double> values) {
    if (values.size() != SF_NUM) {
        std::cerr << "Error: Invalid number of UF values provided." << std::endl;
        return;
    }

    int i = 0;
    for (auto value : values) {
        this->ufValues[i] = value;
        i++;
    }
}

// Check if any component has not available utilization
bool UtilizationFactor::isFull() const {
    for (int i = 0; i < SF_NUM; i++)
        if (ufValues[i] >= 1.0)
            return true;
    return false;
}

// Check if at least one of UF values is used
bool UtilizationFactor::isUsed() const {
    for (int i = 0; i < SF_NUM; i++)
        if (ufValues[i] > UFTHRES)
            return true;
    return false;
}

// Get max UF between all SFs
double UtilizationFactor::getMax() const {
    double maxValue = 0.0;
    for (int i = 0; i < SF_NUM; i++)
        if (this->ufValues[i] >= maxValue)
            maxValue = this->ufValues[i];
    return maxValue;
}

void UtilizationFactor::setSFValue(uint sf, double ufValue) {
    if (sf-7 < SF_NUM) 
        ufValues[sf-7] = ufValue;
    else
        std::cerr << "Invalid SF index. SF = " << sf << std::endl;
}
// Summation operator
UtilizationFactor UtilizationFactor::operator+(const UtilizationFactor& other) const {
    UtilizationFactor result;
    for (int i = 0; i < SF_NUM; i++)
        result.ufValues[i] = this->ufValues[i] + other.ufValues[i];
    return result;
}

UtilizationFactor& UtilizationFactor::operator+=(const UtilizationFactor& other) {
    for (int i = 0; i < SF_NUM; i++)
        this->ufValues[i] += other.ufValues[i];
    return *this;
}

UtilizationFactor& UtilizationFactor::operator-=(const UtilizationFactor& other) {
    for (int i = 0; i < SF_NUM; i++)
        this->ufValues[i] -= other.ufValues[i];
    return *this;
}

// Comparison operators
bool UtilizationFactor::operator==(const UtilizationFactor& other) const {
    for (int i = 0; i < SF_NUM; i++)
        if (this->ufValues[i] != other.ufValues[i])
            return false;
    return true;
}

bool UtilizationFactor::operator!=(const UtilizationFactor& other) const {
    return !(*this == other);
}

bool UtilizationFactor::operator<(const UtilizationFactor& other) const {
    for (int i = 0; i < SF_NUM; i++)
        if (this->ufValues[i] >= other.ufValues[i])
            return false;
    return true;
}

bool UtilizationFactor::operator<=(const UtilizationFactor& other) const {
    return (*this < other) || (*this == other);
}

bool UtilizationFactor::operator>(const UtilizationFactor& other) const {
    return !(*this <= other);
}

bool UtilizationFactor::operator>=(const UtilizationFactor& other) const {
    return !(*this < other);
}

void UtilizationFactor::printUFValues() {
    std::cout << this->ufValues[0];
    for (int i = 1; i < SF_NUM; i++)
        std::cout << ", " << this->ufValues[i];
    std::cout << std::endl;
}