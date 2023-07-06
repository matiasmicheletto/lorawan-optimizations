#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include "../util/util.h"
#include "../model/instance.h"
#include "../model/objective.h"
#include "results.h"

enum MIN {GW, E, UF}; // Greedy minimization methods

OptimizationResults greedy(Instance* l, Objective* o, MIN minimize, bool verbose = false);

#endif // GREEDY_H