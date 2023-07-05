#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include "../util/util.h"
#include "../instance/instance.h"
#include "../objective/objective.h"
#include "results.h"

OptimizationResults greedy(Instance* l, Objective* o, bool verbose = false);

#endif // GREEDY_H