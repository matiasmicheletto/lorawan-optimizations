#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H

#include <chrono>
#include <vector>
#include <math.h>
#include "../util/util.h"
#include "../random/uniform.h"
#include "../model/instance.h"
#include "../model/objective.h"
#include "results.h"

OptimizationResults randomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose = false);
OptimizationResults improvedRandomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose = false);

#endif // RANDOMSEARCH_H