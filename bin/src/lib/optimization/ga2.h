#include "../model/objective.h"
#include "../custom_ga/ga.h"
#include "../custom_ga/moga.h"
#include <thread>


using namespace custom_ga;

struct EdSf { // End device and SF
    uint ed; // ED index
    uint sf; // Values from 7 to 12
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
                const uint randgw = uniform.random(reachableGWs.size());
                const uint gw = reachableGWs[randgw];
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
            for (uint i = 0; i < gwList.size(); i++) {
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
            uint edCount = o->getInstance()->edCount;
            uint* gw = new uint[edCount];
            uint* sf = new uint[edCount];
            for (uint i = 0; i < edCount; i++) {
                gw[i] = -1;
                sf[i] = -1;
            }
            for (uint g = 0; g < gwList.size(); g++) {
                for (EdSf edSf : gwList[g]) {
                    gw[edSf.ed] = g;
                    sf[edSf.ed] = edSf.sf;
                }
            }

            Instance *l = o->getInstance();
            for (uint e = 0; e < edCount; e++) {
                if(gw[e] == -1) {
                    std::vector<uint> glist = l->getGWList(e); // Valid gws for this ed
                    const uint randgw = rand() % glist.size();
                    const uint minSF = l->getMinSF(e, glist[randgw]);
                    const uint maxSF = l->getMaxSF(e);
                    const uint randsf = minSF + rand() % (maxSF - minSF + 1);
                    gw[e] = randgw;
                    sf[e] = randsf;
                    std::this_thread::sleep_for(std::chrono::nanoseconds(50));
                }
            }

            cost = o->eval(gw, sf, gwCount, energy, totalUF, feasible);
        }

        void pushElToGene(uint gw, EdSf edSf) { // Add an ED to a GW
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
            
            // Move an ED from one GW to another
            uint gw1 = (uint) floor(uniform.random(gwList.size()));
            uint gw2 = (uint) floor(uniform.random(gwList.size()));
            uint ed = (uint) floor(uniform.random(gwList[gw1].size()));
            if(ed > 0){
                EdSf edSf = gwList[gw1][ed];
                gwList[gw1].erase(gwList[gw1].begin() + ed);
                gwList[gw2].push_back(edSf);
            }
                
            */
        }

        void singlePointCrossover(Chromosome* other) override {
            GWAllocationChromosome* ot = (GWAllocationChromosome*) other;
            uint pivot = (uint) floor(uniform.random(gwList.size()));
            for(uint gw = 0; gw < pivot; gw++){
                std::swap(gwList[gw], ot->gwList[gw]);
            }
        }

        void doublePointCrossover(Chromosome* other) override {
            this->singlePointCrossover(other);
        }

        void uniformCrossover(Chromosome* other) override {
            this->singlePointCrossover(other);
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

