#include "chromosome.h"

Chromosome::Chromosome(const Chromosome& ch) : genes(ch.genes) {
    mutProb = ch.mutProb;
    // set default crossover method
    setCrossoverMethod(&Chromosome::singlePointCrossover);
}

Chromosome::~Chromosome() {
    for(Gene* gene : genes)
        delete gene;
} 

void Chromosome::mutate() { // Mutate each gene with a probability of 1/genes.size()
    for (unsigned int i = 0; i < genes.size(); i++) {
        if (uniform.random() < mutProb)
            genes[i]->randomize();
    }
}

void Chromosome::crossover(Chromosome* other) { // Crossover method
    crossoverMethod(this, other);
}

void Chromosome::setCrossoverMethod(std::function<void(Chromosome*, Chromosome*)> method) {
    this->crossoverMethod = method;
}

void Chromosome::singlePointCrossover(Chromosome* other) { // Single point crossover
    unsigned int pivot = (unsigned int) floor(uniform.random(genes.size()));
    // From 0 to pivot, swap genes
    for (unsigned int i = 0; i < pivot; i++){
        std::swap(genes[i], other->genes[i]);
    }
}

void Chromosome::doublePointCrossover(Chromosome* other) { // Double point crossover
    unsigned int pivot1 = (unsigned int) floor(uniform.random(genes.size()));
    unsigned int pivot2 = (unsigned int) floor(uniform.random(genes.size()));
    if (pivot1 > pivot2)
        std::swap(pivot1, pivot2);
    // From 0 to pivot1, swap genes
    for (unsigned int i = 0; i < pivot1; i++){
        std::swap(genes[i], other->genes[i]);
    }
    // From pivot1 to pivot2, swap genes
    for (unsigned int i = pivot1; i < pivot2; i++){
        std::swap(genes[i], other->genes[i]);
    }
}

void Chromosome::uniformCrossover(Chromosome* other) { // Uniform crossover
    for (unsigned int i = 0; i < genes.size(); i++) {
        if (uniform.random() < 0.5)
            std::swap(genes[i], other->genes[i]);
    }
}

void Chromosome::printGenotype() const { 
    // Print the genotype of the chromosome. 
    for (unsigned int i = 0; i < genes.size(); i++)
        genes[i]->print();
    std::cout << std::endl;
}