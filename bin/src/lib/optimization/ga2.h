#include "../custom_ga/uniform.h"
#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"


using namespace custom_ga;

struct EdSf { // End device and SF
    unsigned int ed; // ED index
    unsigned int sf; // Values from 7 to 12
};


class GWAllocationChromosome : public Chromosome { // Network allocation (GW and SF for each ED)
    public:
        GWAllocationChromosome(Objective* o) : Chromosome(o->getInstance()->edCount) {
            this->o = o;
            Instance *l = o->getInstance();
            for(uint gw = 0; gw < l->gwCount; gw++) {
                gwList.push_back(std::vector<EdSf>());
            }            
            for(uint ed = 0; ed < l->edCount; ed++) {
                std::vector<uint> reachableGWs = l->getGWList(ed); // Valid gws for this ed
                if(reachableGWs.size() == 0) {
                    std::cout << "No GW for ED " << ed << std::endl;
                    exit(1);
                }
                // Choose a random gw from the list of reachable gws
                const unsigned int gwIndex = uniform.random(reachableGWs.size());
                const unsigned int gw = reachableGWs[gwIndex];
                const uint minSF = l->getMinSF(ed, gw);
                const uint maxSF = l->getMaxSF(ed);
                // Put the ed in the gw
                EdSf edSf = {ed, uniform.random(minSF, maxSF)};
                // EdSf edSf = {ed, minSF};
                gwList[gw].push_back(edSf);
            }
        }

        std::string getName() const override {
            return "Custom GW allocation array";
        }

        void printGenotype() const override {
            std::cout << "Genotype: ";
            for (unsigned int i = 0; i < gwList.size(); i++) {
                std::cout << "GW " << i << ": ";
                for (EdSf edSf : gwList[i]) {
                    std::cout << edSf.ed << "[" << edSf.sf << "] ";
                }
                std::cout << std::endl;
            }    
        }

        void getPhenotype(double &cost, uint &gwCount, uint &energy, double &totalUF, bool &feasible) const {
            //std::cout << "Evaluating phenotype" << std::endl;

            // Evaluate cost and objectives
            unsigned int edCount = o->getInstance()->edCount;
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (unsigned int i = 0; i < edCount; i++) {
                gw[i] = -1;
                sf[i] = -1;
            }
            for (unsigned int i = 0; i < gwList.size(); i++) {
                for (EdSf edSf : gwList[i]) {
                    gw[edSf.ed] = i;
                    sf[edSf.ed] = edSf.sf;
                }
            }
            cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);

            //std::cout << "Cost=" << cost << ", GW=" << gwCount << ", E=" << energy << ", U=" << totalUF << std::endl;
        }

        void pushElToGene(unsigned int gw, EdSf edSf) { // Add an ED to a GW
            if (gw <= gwList.size()) {
                gwList[gw].push_back(edSf);   
            }
        }

        void printPhenotype() const override {
            double cost;
            uint gwCount;
            uint energy;
            double totalUF;
            bool feasible;
            getPhenotype(cost, gwCount, energy, totalUF, feasible);
            std::cout << "Cost=" << cost
                << (feasible ? " (Feasible)" : " (Unfeasible)")
                << ", (GW=" << gwCount 
                << ",E=" << energy 
                << ",U=" << totalUF
                << ")" << std::endl;
        }

        void mutate() override {
            /*
            std::cout << "Mutating..." << std::endl;
            // Move an ED from one GW to another
            unsigned int gw1 = (unsigned int) floor(uniform.random(gwList.size()));
            unsigned int gw2 = (unsigned int) floor(uniform.random(gwList.size()));
            unsigned int ed = (unsigned int) floor(uniform.random(gwList[gw1].size()));
            if(ed > 0){
                EdSf edSf = gwList[gw1][ed];
                gwList[gw1].erase(gwList[gw1].begin() + ed);
                gwList[gw2].push_back(edSf);
            }
            */
        }

        void singlePointCrossover(Chromosome* other) override {
            /*
            std::cout << "Crossover..." << std::endl;

            GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            Instance *l = o->getInstance();
            unsigned int pivot = (unsigned int) floor(uniform.random(gwList.size()));

            for(unsigned int gw = 0; gw < pivot; gw++){
                std::vector<EdSf> temp = gwList[gw];
                gwList[gw] = ot->gwList[gw];
                ot->gwList[gw] = temp;
            }
            
            std::cout << "Crossover done." << std::endl;
            */
        }

        void doublePointCrossover(Chromosome* other) override {
            
        }

        void uniformCrossover(Chromosome* other) override {
            
        }

        void clone(const Chromosome* other) { // Copy the genes from another chromosome
            const GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            this->gwList = ot->getGeneList();
            fitness = other->fitness;
            objectives = other->objectives;
        }

        std::vector<std::vector<EdSf>> getGeneList() const {
            return gwList;
        }
    
    private:
        Objective* o;
        std::vector<std::vector<EdSf>> gwList; // List of EDs assigned to each GW
};

