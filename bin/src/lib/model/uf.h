#ifndef UF_H
#define UF_H

#include <iostream>

// UF value for a GW to be considered as "used"
#define UFTHRES 1e-6
#define SF_NUM 6

class UtilizationFactor {
public:
    UtilizationFactor();
    UtilizationFactor(uint sf, double ufValue);
    UtilizationFactor(std::initializer_list<double> values);
    
    bool isFull() const;
    bool isUsed() const;
    double getMax() const;

    void setSFValue(uint sf, double ufValue);

    // Logic and algebraic operators for UF
    UtilizationFactor operator+(const UtilizationFactor& other) const;
    UtilizationFactor& operator+=(const UtilizationFactor& other);
    UtilizationFactor& operator-=(const UtilizationFactor& other);
    bool operator==(const UtilizationFactor& other) const;
    bool operator!=(const UtilizationFactor& other) const;
    bool operator<(const UtilizationFactor& other) const;
    bool operator<=(const UtilizationFactor& other) const;
    bool operator>(const UtilizationFactor& other) const;
    bool operator>=(const UtilizationFactor& other) const;

    inline double getUFValue(uint sf){ return this->ufValues[sf-7]; };

    void printUFValues();

private:
    double ufValues[SF_NUM];
};

#endif // UF_H