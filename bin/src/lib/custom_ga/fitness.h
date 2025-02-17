#ifndef FITNESS_H
#define FITNESS_H

#include "chromosome.h"


enum CROSS_METHOD {
    C_UNIFORM,
    SINGLE_POINT,
    DOUBLE_POINT
};


class Fitness { // Abstract class that models a fitness function
    public:
        virtual ~Fitness() = default;
        virtual std::string getName() const = 0;        
        virtual void evaluate(Chromosome *chromosome) const = 0;
        virtual Chromosome* generateChromosome(CROSS_METHOD cm) const = 0;

    protected:
        Fitness() = default;
};

#endif // FITNESS_H