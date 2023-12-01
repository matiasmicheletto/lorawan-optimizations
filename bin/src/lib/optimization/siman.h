#ifndef SIMAN_H
#define SIMAN_H

#define STEP_SIZE 1.0           /* step size: number of allocation to alter on each step*/
#define INIT_TRIES 100         /* initialization tries for G4 algorithm */
#define N_TRIES 200             /* how many points do we try before stepping */
#define K 1.0                   /* Boltzmann constant */
#define T_INITIAL 5000.0        /* initial temperature */
#define MU_T 1.002              /* damping factor for temperature */
#define T_MIN 5.0e-1            /* minimal temperature */

#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_siman.h>
#include <gsl/gsl_ieee_utils.h>

#include "../random/uniform.h"
#include "../model/instance.h"
#include "../model/objective.h"
#include "results.h"

OptimizationResults siman(Instance* l, Objective* o, uint iters, bool verbose = false, bool wst = false);

#endif // SIMAN_H