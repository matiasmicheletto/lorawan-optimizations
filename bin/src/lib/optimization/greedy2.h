#ifndef GREEDY2_H
#define GREEDY2_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include "../util/util.h"
#include "../model/instance.h"
#include "../model/objective.h"
#include "results.h"

OptimizationResults greedy2(Instance* l, Objective* o, bool verbose = false);

#endif // GREEDY2_H