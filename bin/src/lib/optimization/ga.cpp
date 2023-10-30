#include "ga.h"
#include "openGA.hpp"

// Global poiters to problem instance and objective function
Instance* _l;
Objective* _o;

// Chromosome model
struct Chromosome {
	std::vector<uint> gw;
	std::vector<uint> sf;
	Chromosome() {
		const uint size = _l->getEDCount();
		gw.resize(size);
		sf.resize(size);
	}
	void print() const { 
        _o->printSolution(gw.data(), sf.data());
	}
};

struct MiddleCost {
	//double total_cost;
	double gwCount;
	double energy;
    double totalUF;
};

typedef EA::Genetic<Chromosome,MiddleCost> GA_Type;
typedef EA::GenerationType<Chromosome,MiddleCost> Generation_Type;

void copy_genes(const Chromosome& X_from, Chromosome& X_to) {
    X_to.gw = X_from.gw;
    X_to.sf = X_from.sf;
}

void randomize_gene(Chromosome& p, const uint index, const std::function<double(void)> &rnd01) {
	// Randomly modify GW and SF allocation for gene number "index"
	std::vector<uint> gwList = _l->getGWList(index); // Valid gws for this ED	
	const uint gw = gwList[(uint)floor(rnd01()*(double)gwList.size())]; // Pick random GW from list
	uint maxSF = _l->getMaxSF(index);
	uint minSF = _l->getMinSF(index, gw);
	p.gw[index] = gw;
	p.sf[index] = (uint)floor(rnd01()*(double)(maxSF - minSF) + (double)minSF);	
	/*
	if(p.sf[index] > 12){
		std::cout << "minSF=" << minSF << std::endl;
		std::cout << "maxSF=" << maxSF << std::endl;
		std::cout << "SF=" << p.sf[index] << std::endl << std::endl;
	}
	*/
}

void init_genes_random(Chromosome& p, const std::function<double(void)> &rnd01) {	
	for(uint i = 0; i < _l->getEDCount(); i++)
		randomize_gene(p, i, rnd01);
}

void init_genes_greedy(Chromosome& p, const std::function<double(void)> &rnd01) {
	const uint gwCount = _l->getGWCount();
    const uint edCount = _l->getEDCount();        
    
	// Create the list of gw indexes and randomize it
	std::vector<uint>gwList(gwCount);
	for(uint i = 0; i < gwCount; i++)
		gwList[i] = i;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(gwList.begin(), gwList.end(), gen);

	std::vector<uint>allocatedEDs;
    for(uint gi = 0; gi < gwList.size(); gi++){ // Allocate end devices
		const uint g = gwList[gi];
        UtilizationFactor guf;        
        for(uint s = 7; s <= 12; s++){ // For each spread factor        
            std::vector<uint> edList = _l->getEDList(g, s);                
            for(uint e = 0; e < edList.size(); e++){
                uint selectedED = edList[e];
                if(std::find(allocatedEDs.begin(), allocatedEDs.end(), selectedED) == allocatedEDs.end()){ // If not allocated                    
                    UtilizationFactor u = _l->getUF(selectedED, s);                    
                    if(!(guf + u).isFull()){ // If allowed to allocate (enough uf)
                        allocatedEDs.push_back(selectedED);
                        p.gw[selectedED] = g;
                        p.sf[selectedED] = s;
                        guf += u;
                    }
                }
                if(allocatedEDs.size() == edCount) return;
            }            
        }
    }
}

Chromosome mutate(const Chromosome& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	const double pr = 1.5 / (double)_l->getEDCount();
	Chromosome X_new;		
	for(uint i = 0; i < _l->getEDCount(); i++){
		if(rnd01() < pr){			
			randomize_gene(X_new, i, rnd01);
		}else{
			X_new.gw[i] = X_base.gw[i];
			X_new.sf[i] = X_base.sf[i];
		}
	}
	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2, const std::function<double(void)> &rnd01) {
	Chromosome X_new;	
	const uint x_point = (uint) (rnd01()*((double)_l->getEDCount()-1)) + 1; // Crossover point
    for(uint i = 0; i < x_point; i++){
        X_new.gw[i] = X1.gw[i];
		X_new.sf[i] = X1.sf[i];
	}
    for(uint i = x_point; i < _l->getEDCount(); i++){
        X_new.gw[i] = X2.gw[i];
		X_new.sf[i] = X2.sf[i];
	}
	return X_new;
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
    uint gwCount, energy;
    double totalUF;
	bool feasible;
    _o->eval(p.gw.data(), p.sf.data(), gwCount, energy, totalUF, feasible);
	c.gwCount = (double) gwCount;
	c.energy = (double) energy;
	c.totalUF = totalUF;
	return feasible; // Reject if not feasible solution ?
	//return true;
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
    return _o->tp.alpha * X.middle_costs.gwCount + 
		_o->tp.beta * X.middle_costs.energy + 
		_o->tp.gamma * X.middle_costs.totalUF;    
}

std::vector<double> calculate_MO_objectives(const GA_Type::thisChromosomeType &X) {
	return {
		X.middle_costs.gwCount,
		X.middle_costs.energy,
		X.middle_costs.totalUF
	};
}


void SO_report_generation_verbose(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
    std::cout << "Generation [" << generation_number << "], "
		      << "Best total cost = " << last_generation.best_total_cost << ", "
		      << "Average cost = " << last_generation.average_cost << std::endl;

	//last_generation.chromosomes[last_generation.best_chromosome_index].genes.print();
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {}

void MO_report_generation_verbose(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const std::vector<uint>& pareto_front) {
    std::cout << "Generation [" << generation_number << "], "
		<< "Best=" << last_generation.best_total_cost << ", "
		<< "Average cost=" << last_generation.average_cost << std::endl;

	std::cout << "Pareto-Front {";
	for(uint i = 0; i < pareto_front.size(); i++){
		std::cout << (i>0?",":"");
		std::cout << pareto_front[i];
	}
	std::cout << "}" << std::endl;
}

void MO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const std::vector<uint>& pareto_front) {}

OptimizationResults ga(Instance* l, Objective* o, const GAConfig& config, bool verbose, bool wst) {

	if(verbose)
        std::cout << std::endl << "--------------- GA ---------------" << std::endl << std::endl;

    _l = l;
    _o = o;

    EA::Chronometer timer;
	timer.tic();

	// Set GA configuration
	GA_Type ga_obj;
	ga_obj.problem_mode = EA::GA_MODE::SOGA;
	ga_obj.multi_threading = true;
	ga_obj.idle_delay_us = 1; // switch between threads quickly
	ga_obj.verbose = false; // prints too much data
	ga_obj.population = config.popsize;
	ga_obj.generation_max = config.maxgen;
	ga_obj.tol_stall_average = 0.001;
	ga_obj.tol_stall_best = 0.001;
	ga_obj.best_stall_max = 100;
    ga_obj.average_stall_max = 100;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes_greedy;	
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    
	ga_obj.SO_report_generation = verbose ? SO_report_generation_verbose : SO_report_generation;
    //ga_obj.elite_count = 10;
	ga_obj.crossover_fraction = config.crossover;
	ga_obj.mutation_rate = config.mutation;
    
	// Print GA configuration
	if(verbose) {
		std::cout << "Population: " << ga_obj.population << std::endl;
		std::cout << "Generations: " << ga_obj.generation_max << std::endl;
		std::cout << "Best stall max.: " << ga_obj.best_stall_max << std::endl;
		std::cout << "Avg stall max.: " << ga_obj.average_stall_max << std::endl;
		std::cout << "Crossover fraction: " << ga_obj.crossover_fraction << std::endl;
		std::cout << "Mutation rate: " << ga_obj.mutation_rate << std::endl;
		std::cout << "Progress:" << std::endl;
	}

	EA::StopReason res = ga_obj.solve(); // Start optimizer

	// Extract optimum
	Chromosome best;
	const int bestIndex = ga_obj.last_generation.best_chromosome_index;
	copy_genes(ga_obj.last_generation.chromosomes[bestIndex].genes, best);

	if(wst) o->exportWST(best.gw.data(), best.sf.data());

	OptimizationResults results;
    results.cost = o->eval(best.gw.data(), best.sf.data(), results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = timer.toc()*1000;
    results.ready = true; // Set export flag to ready

	// Print and return results
	if(verbose){
		std::cout << "Exit condition: ";
		switch (res) {
			case EA::StopReason::MaxGenerations:
				std::cout << " Max. generations" << std::endl;
				break;
			case EA::StopReason::StallAverage:
				std::cout << " Average stall" << std::endl;
				break;
			case EA::StopReason::StallBest:
				std::cout << " Best stall" << std::endl;
				break;
			default:
				break;
		}
		std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
		std::cout << std::endl << "Optimal result:" << std::endl;
		o->printSolution(best.gw.data(), best.sf.data(), true, true);
	}

	return results;
}


OptimizationResults nsga(Instance* l, Objective* o, const GAConfig& config, bool verbose, bool wst) {

	if(verbose)
        std::cout << std::endl << "--------------- NSGA ---------------" << std::endl << std::endl;

    _l = l;
    _o = o;

    EA::Chronometer timer;
	timer.tic();

	// Set GA configuration
	GA_Type ga_obj;
	ga_obj.problem_mode = EA::GA_MODE::NSGA_III;
	ga_obj.multi_threading = true;
	ga_obj.idle_delay_us = 1; // switch between threads quickly
	ga_obj.verbose = false; // prints too much data
	ga_obj.population = config.popsize;
	ga_obj.generation_max = config.maxgen;
	ga_obj.tol_stall_average = 0.001;
	ga_obj.tol_stall_best = 0.001;
	ga_obj.best_stall_max = 100;
    ga_obj.average_stall_max = 100;	
	ga_obj.calculate_MO_objectives = calculate_MO_objectives;
	//ga_obj.init_genes = init_genes_random;
	ga_obj.init_genes = init_genes_greedy;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    	
	ga_obj.MO_report_generation = verbose ? MO_report_generation_verbose : MO_report_generation; 
	ga_obj.crossover_fraction = config.crossover;
	ga_obj.mutation_rate = config.mutation;
    
	// Print GA configuration
	if(verbose) {
		std::cout << "Population: " << ga_obj.population << std::endl;
		std::cout << "Generations: " << ga_obj.generation_max << std::endl;
		std::cout << "Best stall max.: " << ga_obj.best_stall_max << std::endl;
		std::cout << "Avg stall max.: " << ga_obj.average_stall_max << std::endl;
		std::cout << "Crossover fraction: " << ga_obj.crossover_fraction << std::endl;
		std::cout << "Mutation rate: " << ga_obj.mutation_rate << std::endl;
		std::cout << "Progress:" << std::endl;
	}

	EA::StopReason res = ga_obj.solve(); // Start optimizer

	std::cout << "Optimization finished. Printing results..." << std::endl;


	std::cout << ga_obj.last_generation.best_chromosome_index << std::endl;

	// Extract optimum
	Chromosome best;
	//copy_genes(ga_obj.last_generation.chromosomes[ga_obj.last_generation.best_chromosome_index].genes, best);

	if(wst) o->exportWST(best.gw.data(), best.sf.data());

	OptimizationResults results;
    results.cost = o->eval(best.gw.data(), best.sf.data(), results.gwUsed, results.energy, results.uf, results.feasible);
    results.tp = o->tp;
    results.execTime = timer.toc()*1000;
    results.ready = true; // Set export flag to ready

	// Print and return results
	if(verbose){
		std::cout << "Exit condition: ";
		switch (res) {
			case EA::StopReason::MaxGenerations:
				std::cout << " Max. generations" << std::endl;
				break;
			case EA::StopReason::StallAverage:
				std::cout << " Average stall" << std::endl;
				break;
			case EA::StopReason::StallBest:
				std::cout << " Best stall" << std::endl;
				break;
			default:
				break;
		}
		std::cout << "Optimization finished in " << results.execTime << " ms" << std::endl;
		std::cout << std::endl << "Result:" << std::endl;
		o->printSolution(best.gw.data(), best.sf.data(), true, true);
	}

	return results;
}