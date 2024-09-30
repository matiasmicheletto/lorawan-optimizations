#ifndef UNIFORM2_H
#define UNIFORM2_H

#include <random>

class Uniform2 {
    public:
        Uniform2();
        ~Uniform2();
        double random();
        double random(double to);
        double random(double from, double to);

    private:
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<> dis;
};


#endif // UNIFORM2_H