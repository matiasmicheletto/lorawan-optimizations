#include "ga.h"

GeneticAlgorithm::GeneticAlgorithm() { 
    // Initialize with default configuration
    config = new GAConfig();
    // Cannot initialize with default constructor
}

GeneticAlgorithm::GeneticAlgorithm(Fitness *fitnessFunction, GAConfig *config) {
    // Initialize with a fitness function and configuration
    this->config = config;
    this->fitnessFunction = fitnessFunction;
    initialize();
}

GeneticAlgorithm::~GeneticAlgorithm() {
    if(fitnessFunction != nullptr)
        delete fitnessFunction;
    clearPopulation();
}

void GeneticAlgorithm::setConfig(GAConfig *config) {
    this->config = config;
    initialize();
}

void GeneticAlgorithm::setPopulation(std::vector<Chromosome*> pop) {
    clearPopulation();

    this->population = pop;
    
    config->populationSize = pop.size();
    
    elite = config->elitismRate * (double) config->populationSize;
    bestChromosome = fitnessFunction->generateChromosome(this->config->crossoverMethod);

    evaluation();
    sortPopulation();

    status = STATUS::IDLE;
}

void GeneticAlgorithm::sortPopulation() {
    std::sort(population.begin(), population.end(), [](Chromosome* a, Chromosome* b) {
        return a->fitness > b->fitness; // Sort in descending order
    });
}

void GeneticAlgorithm::initialize(){

    if(fitnessFunction == nullptr){
        std::cerr << "Initialization: Fitness function not set" << std::endl;
        return;
    }
    clearPopulation();
    for (unsigned int i = 0; i < config->populationSize; i++) {
        Chromosome *ch = fitnessFunction->generateChromosome(this->config->crossoverMethod);
        population.push_back(ch);
    }
    sortPopulation(); // Sort the population by fitness best to worse

    // Calculate the number of elite individuals
    elite = config->elitismRate * (double) config->populationSize;

    // This is not a pointer to the best in the population, to avoid losing the best individual
    // during the evolution
    bestChromosome = fitnessFunction->generateChromosome(this->config->crossoverMethod);

    status = STATUS::IDLE;
}

void GeneticAlgorithm::clearPopulation() {
    population.clear();
}

void GeneticAlgorithm::evaluation() {
    long int bestFitnessIndex = -1;
    for (unsigned int i = 0; i < config->populationSize; i++) {
        fitnessFunction->evaluate(population[i]);
        if(population[i]->fitness > bestFitnessValue){
            //std::cout << "New best fitness: " << population[i]->fitness << std::endl;
            bestFitnessValue = population[i]->fitness;
            bestFitnessIndex = i;
        }
    }
    
    if(bestFitnessIndex != -1){
        bestChromosome->clone(population[bestFitnessIndex]);
    }else{
        stagnatedGenerations++;
    }
}

void GeneticAlgorithm::selection() { // Roulette wheel selection

    std::vector<Chromosome*> newPopulation;

    // Selection requires the fitness values to be positive
    double fitnessSum = 0.0;
    for (unsigned int j = 0; j < config->populationSize; j++) {
        fitnessSum += population[j]->fitness;
    }

    // Select the best individuals between the rest of the population
    //unsigned int tries = 0; // Avoid infinite loop (should never happen)
    while(newPopulation.size() < config->populationSize){
        const double r = uniform.random(fitnessSum);
        double sum = 0.0;
        for (unsigned int j = 0; j < config->populationSize; j++) {
            sum += population[j]->fitness;
            if (sum >= r) {
                // Create new chromosome (already evaluated)
                Chromosome *ch = fitnessFunction->generateChromosome(this->config->crossoverMethod);
                ch->clone(population[j]);
                newPopulation.push_back(ch);
                break;
            }
        }
    }

    for(unsigned int i = 0; i < config->populationSize; i++){
        delete population[i];
        population[i] = newPopulation[i];
    }
}

void GeneticAlgorithm::crossover() {
    // At this point, the population is sorted, so elite individuals are not included in the crossover
    for (unsigned int i = elite; i < config->populationSize; i++) {
        if (uniform.random() < config->crossoverRate) {
            unsigned int parent1 = uniform.random(config->populationSize - elite) + elite;
            population[i]->crossover(population[parent1]);
        }
    }
}

void GeneticAlgorithm::mutation() {
    for (unsigned int i = elite; i < config->populationSize; i++) {
        if (uniform.random() < config->mutationRate) {
            population[i]->mutate();
        }
    }
}

void GeneticAlgorithm::print() {
    
    if(config->printLevel < 0 || config->printLevel > 3){
        std::cerr << "Invalid print level" << std::endl;
        return;
    }

    if(config->printLevel >= 0)
        config->print();

    if(config->printLevel >= 1){
        if(population.size() == 0){
            std::cout << "Population not initialized" << std::endl;
            return;
        }else{
            std::cout << "Best fitness: " << population[0]->fitness << std::endl;
            std::cout << "Best chromosome:" << std::endl;
            std::cout << "  - ";
            population[0]->printGenotype();
            std::cout << "  - ";
            population[0]->printPhenotype();
            std::cout << std::endl;
        }
    }
    if(config->printLevel >= 2){
        std::cout << "Population fitness: " << std::endl;
        for (unsigned int i = 0; i < config->populationSize; i++) {
            std::cout << "Chromosome " << i << ": " << population[i]->fitness << std::endl;
        }
    }
    if(config->printLevel >= 3){
        std::cout << "Population genes: " << std::endl;
        for (unsigned int i = 0; i < config->populationSize; i++) {
            population[i]->printGenotype();
            population[i]->printPhenotype();
        }
    }
}


GAResults GeneticAlgorithm::run() {
    
    GAResults results(OBJTYPE::SINGLE);

    if(fitnessFunction == nullptr){
        std::cerr << "Run: Fitness function not set" << std::endl;
        return results;
    }
    if(population.size() == 0){
        std::cerr << "Population not initialized" << std::endl;
        return results;
    }
    
    status = STATUS::RUNNING;
    bestFitnessValue = __DBL_MIN__;
    currentGeneration = 0;
    stagnatedGenerations = 0;
    unsigned int maxStagationGenerations = config->stagnationWindow*config->maxGenerations;
    

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    while (status == STATUS::RUNNING){
        // GA steps
        selection(); // Select the best individual by roulette wheel method
        sortPopulation(); // Sort the population from best to worst fitness
        crossover(); // Apply crossover using single point method
        mutation(); // Perform mutation (all individuals are evaluated here)
        evaluation(); // Evaluate the new population

        ///// Check stop conditions ///////

        auto elapsed = std::chrono::high_resolution_clock::now() - start; // Time in milliseconds
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() > config->timeout) {
            //*config->outputStream << "Timeout reached (" << config->timeout << "s)" << std::endl;
            status = STATUS::TIMEOUT;
            break;
        }

        if(stagnatedGenerations > maxStagationGenerations){
            //*config->outputStream << "Stagnation reached: " << stagnatedGenerations << " generations out of " << config->maxGenerations << " stipulated." << std::endl;
            status = STATUS::STAGNATED;
            break;
        }

        currentGeneration++;
        if(currentGeneration >= config->maxGenerations){
            //*config->outputStream << "Max generations reached (" << config->maxGenerations << ")" << std::endl;
            status = STATUS::MAX_GENERATIONS;
            break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // Convert to milliseconds

    // Export results
    results.status = status;
    results.best = bestChromosome;
    results.bestFitnessValue = bestChromosome->fitness;
    results.generations = currentGeneration;
    results.elapsed = static_cast<int>(duration.count());

    /*
    for (unsigned int i = 0; i < config->populationSize; i++) {
        std::cout << "Chromosome " << i << ": ";
        std::cout << " GW: " << population[i]->objectives[0] << ", ";
        std::cout << " E: " << population[i]->objectives[1] << ", ";
        std::cout << " U: " << population[i]->objectives[2] << " --> Fitness: ";
        std::cout << population[i]->fitness << std::endl;
    }
    */
    return results;
}
