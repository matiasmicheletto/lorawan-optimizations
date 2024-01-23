#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H

#include <chrono>
#include <vector>
#include <math.h>
#include "../util/util.h"
#include "../random/uniform.h"
#include "../model/instance.h"
#include "../model/objective.h"

OptimizationResults randomSearch(Instance* l, Objective* o, uint maxIters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults improvedRandomSearch(Instance* l, Objective* o, uint maxIters, uint timeout, bool verbose = false, bool wst = false);

#endif // RANDOMSEARCH_H