#include "ga.h"
#include "openGA.hpp"

// Global poiters to problem instance and objective function
Instance* _l;
Objective* _o;

// Chromosome model
struct Chromosome {
	unsigned int* gw;
	unsigned int* sf;
	void print() const { 
        _o->printSolution(gw, sf);
	}
};

struct MiddleCost {
	float cost;	
};

typedef EA::Genetic<Chromosome,MiddleCost> GA_Type;
typedef EA::GenerationType<Chromosome,MiddleCost> Generation_Type;

void copy_genes(const Chromosome& from, Chromosome& to) {
	std::copy(from.gw, from.gw + _l->getEDCount(), to.gw);
	std::copy(from.sf, from.sf + _l->getEDCount(), to.sf);
}

void randomize_gene(const Chromosome& p, unsigned int index, const std::function<double(void)> &rnd01) {
	// Randomly modify GW and SF allocation for gene number "index"
	std::vector<unsigned int> gwList = _l->getGWList(index); // Valid gws for this ED
	p.gw[index] = gwList.at((unsigned int)floor(rnd01()*gwList.size())); // Pick random GW from list
	// Next, select a valid SF given ED period and selected GW
	unsigned int minSF = _l->getMinSF(index, p.gw[index]);
	unsigned int maxSF = _l->getMaxSF(index);
	p.sf[index] = (unsigned int)floor(rnd01()*(double)(maxSF - minSF) + (double)minSF);
}

void init_genes(Chromosome& p, const std::function<double(void)> &rnd01) {
	// Random gene initialization
	p.gw = (unsigned int*) malloc( sizeof(unsigned int) * _l->getEDCount());
	p.sf = (unsigned int*) malloc( sizeof(unsigned int) * _l->getEDCount());
    for(unsigned int i = 0; i < _l->getEDCount(); i++)
        randomize_gene(p, i, rnd01);
}

Chromosome mutate(const Chromosome& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	Chromosome X_new;
    init_genes(X_new, rnd01);
    copy_genes(X_base, X_new);
	for(unsigned int i = 0; i < _l->getEDCount(); i++)
		if(rnd01() < 1.0 / (double)_l->getEDCount()) // Mutate a single gene
			randomize_gene(X_new, i, rnd01);
	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2, const std::function<double(void)> &rnd01) {
	Chromosome X_new;
    init_genes(X_new, rnd01);
	unsigned int x_point = (unsigned int) (rnd01()*(double)_l->getEDCount()); // Crossover point
    for(unsigned int i = 0; i < x_point; i++){
        X_new.gw[i] = X1.gw[i];
		X_new.sf[i] = X1.sf[i];
	}
    for(unsigned int i = x_point; i < _l->getEDCount(); i++){
        X_new.gw[i] = X2.gw[i];
		X_new.sf[i] = X1.sf[i];
	}
	return X_new;
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
    unsigned int gwCount, energy;
    double totalUF;
    c.cost = _o->eval(p.gw, p.sf, gwCount, energy, totalUF);
	return c.cost < __DBL_MAX__; // Reject if not feasible solution
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
    return X.middle_costs.cost;    
}

void SO_report_generation_verbose(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
    std::cout << "Generation [" << generation_number << "], "
		<< "Best=" << last_generation.best_total_cost << ", "
		<< "Average cost=" << last_generation.average_cost << std::endl;
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {}

void ga(Instance* l, Objective* o, const GAConfig& config, bool verbose) {

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
	ga_obj.verbose = false;
	ga_obj.population = config.popsize;
	ga_obj.generation_max = config.maxgen;
	ga_obj.best_stall_max = config.maxgen/100;
    ga_obj.average_stall_max = config.maxgen/100;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    
	if(verbose)
		ga_obj.SO_report_generation = SO_report_generation_verbose;
	else
		ga_obj.SO_report_generation = SO_report_generation;
    ga_obj.elite_count = 5;
	ga_obj.crossover_fraction = config.crossover;
	ga_obj.mutation_rate = config.mutation;
    
	// Print GA configuration
	if(verbose){
		std::cout << "Population: " << config.popsize << std::endl;
		std::cout << "Generations: " << config.maxgen << std::endl;
		std::cout << "Crossover rate: " << config.crossover << std::endl;
		std::cout << "Mutation rate: " << config.mutation << std::endl;
		std::cout << "Progress:" << std::endl;
	}

	// Start optimizer
	ga_obj.solve();

	Chromosome best = ga_obj.last_generation.chromosomes[ga_obj.last_generation.best_chromosome_index].genes;

	std::cout << std::endl;
	o->printSolution(best.gw, best.sf, true, true);
}