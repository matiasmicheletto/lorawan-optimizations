#ifndef GENE_H
#define GENE_H

#include <iostream>
#include "./uniform.h" //RANDOM
//#include "../misc/custom_random.h"

using namespace custom_ga;

class Gene { // Abstract class that models a gene
    public:
        virtual ~Gene(){}
        virtual void randomize() = 0;
        virtual void print() const = 0;

    protected:
        Gene() = default;
        custom_ga::Uniform uniform; //RANDOM
};

#endif // GENE_H