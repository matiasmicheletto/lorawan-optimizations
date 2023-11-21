#include "siman.h"

// Global poiters to problem instance and objective function
Instance* _lt;
Objective* _ot;
Uniform uniform = Uniform(0.0, 1.0);
uint ED_COUNT = 0;

// Conversion functions
void sol2gwsf(float sol, uint &gw, uint &sf) {
    gw = static_cast<uint>(sol);
    sf = static_cast<uint>((sol - gw) * 10) + 7;
}

float gwsf2sol(uint gw, uint sf) {
    return (float) gw + ((float) (sf-7))/10.0;
}

void randomize_alloc(float *sol, uint size, uint index) {

	// Randomly modify GW and SF allocation for "index"
	std::vector<uint> gwList = _lt->getGWList(index); // Valid gws for this ED	

    // Remove from gwList the ones that are not in sol (to avoid adding new ones)
    for(uint i = 0; i < gwList.size(); i++){
        const uint gwToFind = gwList[i];
        bool found = false;
        for(uint j = 0; j < size; j++){
            uint gw, sf;
            sol2gwsf(sol[i], gw, sf);
            if(gw == gwToFind)
                found = true;
        }
        if(!found) // Remove gw from list if it was not previously used
            gwList.erase(std::remove(gwList.begin(), gwList.end(), gwToFind), gwList.end());
    }
    if(gwList.size() > 1){ // If it is possible to change gw, proceed, else leave everything same as before
        const uint gw = gwList[(uint)floor(uniform.random()*(double)gwList.size())]; // Pick random GW from list
        //uint maxSF = _lt->getMaxSF(index);
        //uint minSF = _lt->getMinSF(index, gw);
        //uint sf = (uint)floor(uniform.random()*(double)(maxSF - minSF) + (double)minSF);
        uint sf = _lt->getMinSF(index, gw);
        sol[index] = gwsf2sol(gw, sf);
    }
}


void init_g4(float* sol, int size) {

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
                    for(uint i = 0; i < edCount; i++)
                        sol[i] = gwsf2sol(gw[i], sf[i]);
                }
            }
        }
    }
}


double Etsp(void *xp) { // Energy computation

    float *sol = (float*) xp;

    uint gwCount, energy;
    double totalUF;
	bool feasible;

    uint gw[ED_COUNT];
    uint sf[ED_COUNT];
    for(uint i = 0; i < ED_COUNT; i++){
        uint gwtemp;
        uint sftemp;
        sol2gwsf(sol[i], gwtemp, sftemp);
        gw[i] = gwtemp;
        sf[i] = sftemp;
    }
    _ot->eval(gw, sf, gwCount, energy, totalUF, feasible);

    // Return cost
    return _ot->tp.alpha * gwCount + _ot->tp.beta * energy + _ot->tp.gamma * totalUF; 
}

bool areEqual(float a, float b, float epsilon = 1e-5) {
    return std::fabs(a - b) < epsilon;
}

double Mtsp(void *xp, void *yp) { // Distance between two configurations
    double distance = 0;
    float* a = (float*) xp;
    float* b = (float*) yp;
    for(uint i = 0; i < ED_COUNT; i++) 
        distance += areEqual(a[i], b[i]) ? 0 : 1;
    return distance;
}

void Stsp(const gsl_rng * r, void *xp, double step_size) { // Step function
    float*sol = (float*) xp;
	for(uint i = 0; i < ED_COUNT; i++)
		if(uniform.random() < (step_size/(double)ED_COUNT))
			randomize_alloc(sol, ED_COUNT, i);
}

void Ptsp(void *xp) {
    /*
    float*sol = (float*) xp;
	for(uint i = 0; i < ED_COUNT; i++)
		std::cout << sol[i] << " ";
    std::cout << std::endl;
    */
}

OptimizationResults siman(Instance* l, Objective* o, uint iters, bool verbose, bool wst) {
    auto start = std::chrono::high_resolution_clock::now();

    if(verbose)
        std::cout << std::endl << "--------------- SIM. ANNEAL. ---------------" << std::endl << std::endl;

    _lt = l;
    _ot = o;
    
    ED_COUNT = l->getEDCount();
    
    //const int ITERS_FIXED = (int)  ((double)ED_COUNT / 10); // Reduce number of iterations if is large ED_COUNT
    const int ITERS_FIXED = iters;

    const gsl_rng * r = gsl_rng_alloc (gsl_rng_env_setup()) ;
    gsl_ieee_env_setup ();

    
    float x_initial[ED_COUNT];
    init_g4(x_initial, ED_COUNT);
    
    gsl_siman_params_t params = {N_TRIES, ITERS_FIXED, STEP_SIZE, K, T_INITIAL, MU_T, T_MIN};

    gsl_siman_solve(r, x_initial, Etsp, Stsp, Mtsp, Ptsp, NULL, NULL, NULL, ED_COUNT*sizeof(float), params);

    // Eval best and export results
    OptimizationResults results;

    uint gw[ED_COUNT];
    uint sf[ED_COUNT];
    for(uint i = 0; i < ED_COUNT; i++){
        uint gwtemp;
        uint sftemp;
        sol2gwsf(x_initial[i], gwtemp, sftemp);
        gw[i] = gwtemp;
        sf[i] = sftemp;
    }

    results.cost = _ot->eval(gw, sf, results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = _ot->tp;
    results.execTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    results.ready = true; // Set export flag to ready

    if(verbose){
		std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
		std::cout << std::endl << "Optimal result:" << std::endl;
		_ot->printSolution(gw, sf, true, true, true);
	}

    return results;
}
