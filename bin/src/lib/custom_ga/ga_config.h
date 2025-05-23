#ifndef GA_CONFIG_H
#define GA_CONFIG_H

#include <iostream>
#include <cstring>

#include "./output_stream.h"
#include "fitness.h"

class GAConfig {
    
    public:
        GAConfig();

        unsigned int populationSize;
        unsigned int maxGenerations;
        double mutationRate;
        double crossoverRate;
        CROSS_METHOD crossoverMethod;
        double elitismRate;
        unsigned int timeout;
        double stagnationWindow;
        int printLevel;
        std::ostream *outputStream;

        void setConfig(int argc, char **argv);
        void print();
};

#endif // GA_CONFIG_H