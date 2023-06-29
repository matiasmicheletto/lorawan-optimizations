#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H

#include <chrono>
#include <vector>
#include <math.h>
#include "../util/util.h"
#include "../random/uniform.h"
#include "../instance/instance.h"
#include "../objective/objective.h"

void randomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose = false);
void improvedRandomSearch(Instance* l, Objective* o, unsigned long maxIters, bool verbose = false);

#endif // RANDOMSEARCH_H