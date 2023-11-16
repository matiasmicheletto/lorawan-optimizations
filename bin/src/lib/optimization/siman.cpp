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

void init_random(Salloc& initial) {	
	for(uint i = 0; i < _lt->getEDCount(); i++)
		randomize_alloc(initial, i);
}

void init_g4(Salloc& initial) {

	const uint gwCount = _lt->getGWCount();
    const uint edCount = _lt->getEDCount();

    double minimumCost = __DBL_MAX__;
    std::vector<std::vector<std::vector<uint>>> clusters; // Clusters tensor (SF x GW x ED)
    clusters.resize(6); // Initialize list of matrices (GW x ED)
    for(uint s = 7; s <= 12; s++){
        
        // Build cluster matrix for this SF
        for(uint g = 0; g < gwCount; g++)
            clusters[s-7].push_back(_lt->getAllEDList(g ,s));
        
        // Check if SF has coverage
        bool hasCoverage = true; // This is updated if an ED cannot be assigned to any GW
        for(uint e = 0; e < edCount; e++){
            bool hasGW = false;
            for(uint g = 0; g < gwCount; g++){
                /* Find if ED "e" is present in the "clusters" tensor
                for(uint ee = 0; ee < clusters[s-7][g].size(); ee++){
                    if(clusters[s-7][g][ee] == e){
                        hasGW = true;
                        break;
                    }
                }
                */
                auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                hasGW = (it != clusters[s-7][g].end());
                if(hasGW) break;
            }
            if(!hasGW){ 
                hasCoverage = false;
                if(s == 12) {
                    std::cout << "Cannot reach coverage" << std::endl;
                    exit(1);
                }
                break;
            }
        }

        if(!hasCoverage) continue; // Next SF
        else{ // Has coverage --> make allocation and eval objective function
            
            // Initialize GW List
            std::vector<uint>gwList(gwCount);
            for(uint i = 0; i < gwCount; i++) gwList[i] = i;

            std::random_device rd;
            std::mt19937 gen(rd());

            for(uint iter = 0; iter < INIT_TRIES; iter++){ // Try many times                
                
                std::shuffle(gwList.begin(), gwList.end(), gen); // Shuffle list of gw

                uint gw[edCount];
                uint sf[edCount];
                
                // Start allocation of EDs one by one
                std::vector<UtilizationFactor> gwuf(gwCount); // Utilization factors of gws
                for(uint e = 0; e < edCount; e++){ 
                    for(uint gi = 0; gi < gwCount; gi++){
                        const uint g = gwList[gi];
                        // Check if ED e can be allocated to GW g
                        auto it = std::find(clusters[s-7][g].begin(), clusters[s-7][g].end(), e);
                        if((it != clusters[s-7][g].end()) && !gwuf[g].isFull()){
                            uint minsf = _lt->getMinSF(e, g);
                            gw[e] = g;
                            sf[e] = minsf; // Always assign lower SF
                            gwuf[g] += _lt->getUF(e, minsf);
                            break; // Go to next ed
                        }
                    }
                }// Allocation finished

                // Eval solution
                uint gwUsed, energy; double uf; bool feasible;
                const double cost = _ot->eval(gw, sf, gwUsed, energy, uf, feasible);
                if(feasible && cost < minimumCost){ // New optimum
                    minimumCost = cost;
                    // Copy current to best
                    for(uint i = 0; i < edCount; i++){
                        initial.gw[i] = gw[i];
                        initial.sf[i] = sf[i];
                    }

                    /*
                    std::copy(gw, gw + edCount, back_inserter(initial.gw));
                    std::copy(sf, sf + edCount, back_inserter(initial.sf));
                    */
                }
            }
        }
    }

    
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
        std::cout << std::endl << "--------------- SIM. ANNEAL. ---------------" << std::endl << std::endl;

    _lt = l;
    _ot = o;

    const gsl_rng * r = gsl_rng_alloc (gsl_rng_env_setup()) ;
    gsl_ieee_env_setup ();

    Salloc* s_initial = new Salloc();
    //init_random(*s_initial);
    init_g4(*s_initial);

    gsl_siman_params_t params = {N_TRIES, (int) iters/10, STEP_SIZE, K, T_INITIAL, MU_T, T_MIN};

    gsl_siman_solve(r, s_initial, Etsp, Stsp, Mtsp, Ptsp, copy_func, construct, destroy, 0, params);

    // Export results

    OptimizationResults results;
    results.cost = _ot->eval(s_initial->gw.data(), s_initial->sf.data(), results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = _ot->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
		std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
		std::cout << std::endl << "Optimal result:" << std::endl;
		_ot->printSolution(s_initial->gw.data(), s_initial->sf.data(), true, true);
	}

    return results;
}
