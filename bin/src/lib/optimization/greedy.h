#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>
#include "../util/util.h"
#include "../model/instance.h"
#include "../model/objective.h"
#include "../model/uf.h"

enum MIN {GW, E, UF}; // Greedy minimization methods

OptimizationResults greedy(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false);
OptimizationResults greedy4(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults greedy8(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);

/*
OptimizationResults greedy1(Instance* l, Objective* o, MIN minimize, bool verbose = false, bool wst = false);
OptimizationResults greedy2(Instance* l, Objective* o, bool verbose = false, bool wst = false);
OptimizationResults greedy3(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);

OptimizationResults greedy5(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults greedy6(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults greedy7(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);

OptimizationResults greedy9(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults greedy10(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);
OptimizationResults greedy11(Instance* l, Objective* o, uint iters, uint timeout);
OptimizationResults greedy12(Instance* l, Objective* o, uint iters, uint timeout);
OptimizationResults greedy13(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false);
OptimizationResults greedy14(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false);
*/
OptimizationResults heuristica_rod(Instance* l, Objective* o, uint iters, uint timeout, bool verbose = false, bool wst = false);

#endif // GREEDY_H