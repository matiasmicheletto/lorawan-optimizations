#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <chrono>
#include "../util/util.h"
#include "../instance/instance.h"
#include "../objective/objective.h"
#include "../optimization/results.h"

OptimizationResults greedy(Instance* l, Objective* o, uint maxIters, bool verbose = false);

#endif // GREEDY_H