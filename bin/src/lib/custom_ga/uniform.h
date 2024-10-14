#ifndef UNIFORM2_H
#define UNIFORM2_H

#include <random>

namespace custom_ga {
    class Uniform {
        public:
            Uniform();
            ~Uniform();
            double random();
            double random(double to);
            double random(double from, double to);

        private:
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_real_distribution<> dis;
    };
}


#endif // UNIFORM2_H