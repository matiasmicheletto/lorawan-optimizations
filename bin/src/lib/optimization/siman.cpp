#include "siman.h"

// Global poiters to problem instance and objective function
Instance* _lt;
Objective* _ot;
Uniform uniform = Uniform(0.0, 1.0);

struct s_allocation { // Solution model
	std::vector<uint> gw;
	std::vector<uint> sf;
	s_allocation() {
		const uint size = _lt->getEDCount();
		gw.resize(size);
		sf.resize(size);
	}
	void print() const { 
        //_ot->printSolution(gw.data(), sf.data());
        //std::cout << std::endl;
	}
};

typedef struct s_allocation Salloc;

void copy_func (void *source, void *dest) {
    Salloc* a = (Salloc*) source;
    Salloc* b = (Salloc*) dest;
    const uint edCount = _lt->getEDCount();

    /*
    std::copy(a->gw, a->gw + edCount, b->gw);
    std::copy(a->sf, a->sf + edCount, b->sf);
    */

    for(uint i = 0; i < edCount; i++) {
        b->gw[i] = a->gw[i];
        b->sf[i] = a->sf[i];
    }
}

void * construct (void *xp) {
    Salloc* a = (Salloc*) xp;
    return a;
}

void destroy (void *xp) {
    // 
}

void randomize_alloc(Salloc& p, const uint index) {
	// Randomly modify GW and SF allocation for "index"
	std::vector<uint> gwList = _lt->getGWList(index); // Valid gws for this ED	
	const uint gw = gwList[(uint)floor(uniform.random()*(double)gwList.size())]; // Pick random GW from list
	uint maxSF = _lt->getMaxSF(index);
	uint minSF = _lt->getMinSF(index, gw);
	p.gw[index] = gw;
	p.sf[index] = (uint)floor(uniform.random()*(double)(maxSF - minSF) + (double)minSF);	
}

void init_random(Salloc& p) {	
	for(uint i = 0; i < _lt->getEDCount(); i++)
		randomize_alloc(p, i);
}

double Etsp(void *xp) { // Energy computation
    Salloc* X = (Salloc*) xp;

    uint gwCount, energy;
    double totalUF;
	bool feasible;
    _ot->eval(X->gw.data(), X->sf.data(), gwCount, energy, totalUF, feasible);

    // Return cost
    return _ot->tp.alpha * gwCount + _ot->tp.beta * energy + _ot->tp.gamma * totalUF; 
}

double Mtsp(void *xp, void *yp) { // Distance between two configurations
    double distance = 0;
    Salloc* a = (Salloc*) xp;
    Salloc* b = (Salloc*) yp;
    for(uint i = 0; i < _lt->getEDCount(); i++) {
        distance += ((a->gw[i] == b->gw[i]) ? 0 : 1);
        distance += ((a->sf[i] == b->sf[i]) ? 0 : 1);
    }
    return distance;
}

void Stsp(const gsl_rng * r, void *xp, double step_size) { // Step function
    Salloc* X = (Salloc*) xp;
	for(uint i = 0; i < _lt->getEDCount(); i++)
		if(uniform.random() < step_size)	
			randomize_alloc(*X, i);
}

void Ptsp(void *xp) {
    Salloc* a = (Salloc*) xp;
    a->print();
}

OptimizationResults siman(Instance* l, Objective* o, uint iters, bool verbose, bool wst) {
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose)
        std::cout << std::endl << "--------------- SIM. ANEAL. ---------------" << std::endl << std::endl;

    _lt = l;
    _ot = o;

    const gsl_rng * r = gsl_rng_alloc (gsl_rng_env_setup()) ;
    gsl_ieee_env_setup ();

    Salloc* s_initial = new Salloc();
    init_random(*s_initial);

    gsl_siman_params_t params = {N_TRIES, (int) iters/10, STEP_SIZE, K, T_INITIAL, MU_T, T_MIN};

    std::cout << "Doing " << iters << " iterations" << std::endl;

    gsl_siman_solve(r, s_initial, Etsp, Stsp, Mtsp, Ptsp, copy_func, construct, destroy, 0, params);

    // Export results

    OptimizationResults results;
    results.cost = o->eval(s_initial->gw.data(), s_initial->sf.data(), results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
		std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
		std::cout << std::endl << "Optimal result:" << std::endl;
		o->printSolution(s_initial->gw.data(), s_initial->sf.data(), true, true);
	}

    return results;
}
