#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include "../instance/instance.h"
#include "../objective/objective.h"

void greedy(Instance* l, Objective* o, unsigned int maxIters, bool verbose = false);

#endif // GREEDY_H