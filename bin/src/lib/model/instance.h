#ifndef INSTANCE_H
#define INSTANCE_H

/* 
    Class Instance: Models problem parameters. Load parameters from input file 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>

#include "uf.h"
#include "../util/util.h"
#include "../random/random.h"
#include "../random/uniform.h"
#include "../random/normal.h"
#include "../random/clouds.h"
#include "../random/custom.h"

#define MAX_TRIES 100000

enum POSDIST {UNIFORM, NORMAL, CLOUDS}; 
enum PERIODIST {SOFT, MEDIUM, HARD, FIXED}; 
enum OUTPUTFORMAT {NONE, TXT, HTML, ALL};

struct InstanceConfig { // This is for instance generation
    uint mapSize; // Map size
    uint edNumber; // Number of end devices (rows)
    uint gwNumber; // Number of gateways (columns)
    bool scaled; // Use alternate function to compute SF from distance
    PERIODIST timeRequirement; // PDF for ED periods
    POSDIST posDistribution; // Distribution of positions
    OUTPUTFORMAT outputFormat; // Format of output file  
    uint fixedPeriod; // Fixed period (if PERIODIST == FIXED)

    InstanceConfig(  // Default configuration parameters
        uint mapSize = 1000,
        uint edNumber = 100,
        uint gwNumber = 20,
        bool scaled = false,
        PERIODIST timeRequirement = SOFT,
        POSDIST posDistribution = UNIFORM,
        OUTPUTFORMAT outputFormat = NONE,
        uint fixedPeriod = 3200
    ) : 
        mapSize(mapSize),
        edNumber(edNumber),
        gwNumber(gwNumber),
        scaled(scaled),
        timeRequirement(timeRequirement),
        posDistribution(posDistribution),
        outputFormat(outputFormat),
        fixedPeriod(fixedPeriod) {}
};

// Models for the two tipes of nodes: end-devices and gateways
struct Position {
    double x;
    double y;
};

struct EndDevice {
    Position pos;
    int period;
};

class Instance { // Provides attributes and funcions related to problem formulation
    public:
        Instance(char* filename); // Load data from file
        Instance(const InstanceConfig& config = InstanceConfig()); // Generate from config
        ~Instance();
        
        void printRawData();
        void exportRawData(const char* filename = nullptr);
        void generateHtmlPlot(const char* filename);
        void copySFDataTo(std::vector<std::vector<uint>>& destination);
        
        uint gwCount, edCount;
        inline char* getInstanceFileName(){return this->instanceFileName;};
        inline uint sf2e(uint sf) {return this->pw[sf-7];};
        uint getMinSF(uint ed, uint gw);
        uint getMaxSF(uint ed);
        UtilizationFactor getUF(uint ed, uint sf);
        uint getPeriod(uint ed);
        std::vector<uint> getGWList(uint ed);
        std::vector<uint> getSortedGWList(uint ed);
        std::vector<uint> getSortedGWListByAvailableEd(uint ed);
        std::vector<uint> getEDList(uint gw, uint sf);
        std::vector<uint> getAllEDList(uint gw, uint maxSF);

    private:
        std::vector<std::vector<uint>> raw;
        std::vector<EndDevice> eds; 
        std::vector<Position> gws; 
        char* instanceFileName;
        OUTPUTFORMAT outputFormat;
        static const uint pw[6];

        uint _getMaxSF(uint period);
        uint _getMinSF(double distance);
        uint _getMinSFScaled(double distance);
};

struct Allocation { // Models a candidate solution (allocation of gw and sf for each ed)
    std::vector<uint> gw;
	std::vector<uint> sf;
    std::vector<bool> connected;
    uint connectedCount;
    std::vector<UtilizationFactor> ufGW; // Total UF of each GW
    Instance *l;

	Allocation(Instance* l) {
		gw.resize(l->edCount);
		sf.resize(l->edCount);
        connected.resize(l->edCount);
        connectedCount = 0;
        ufGW.resize(l->gwCount);
        this->l = l;
	}

    bool checkUFAndConnect(uint e, uint g, uint asf = 0, bool incremental = false) {
        uint sf2 = (asf == 0 ? l->getMinSF(e, g) : asf); // Use provided or min SF as default
        UtilizationFactor nextUF;
        while(sf2 <= 12) {
            nextUF = l->getUF(e, sf2); // UF of node e for g
            if(!(ufGW[g] + nextUF).isFull()){ // If available UF, use it
                gw[e] = g;
                sf[e] = sf2;
                ufGW[g] += nextUF;
                if(!connected[e]){ // May be previously connected
                    connected[e] = true;
                    connectedCount++;
                }
                return true;
            }
            if(incremental)
                sf2++;
            else 
                break;
        }
        return false;
    }

    bool checkUFAndMove(uint e, uint g) {
        if(gw[e] != g && connected[e]){
            const uint sf2 = l->getMinSF(e, g);
            if(sf2 < sf[e]){ // If new SF is smaller
                const UtilizationFactor nextUF = l->getUF(e, sf2); // UF of node e for g
                if(!(ufGW[g] + nextUF).isFull()){ // If g available for e with sf2
                    ufGW[gw[e]] -= l->getUF(e, sf[e]); // Substract previous UF to prev GW
                    gw[e] = g;
                    sf[e] = sf2;
                    ufGW[g] += nextUF;
                    return true;
                }
            }
        }
        return false;
    }

    /*
    void connect(uint e, uint g, int asf = -1) { // Unvalidated operation
        const uint sf2 = (asf == -1 ? l->getMinSF(e, g) : asf); // Use provided or min SF as default
        const UtilizationFactor nextUF = l->getUF(e, sf2);
        gw[e] = g;
        sf[e] = sf2;
        ufGW[g] += nextUF;
        connected[e] = true;
        connectedCount++;
    }

    void move(uint e, uint g, int asf = -1) { // Unvalidated operation
        const uint sf2 = (asf == -1 ? l->getMinSF(e, g) : asf); // Use provided or min SF as default
        ufGW[gw[e]] -= l->getUF(e, sf[e]); // Substract previous UF to prev GW
        gw[e] = g;
        sf[e] = sf2;
        connected[e] = true;
        ufGW[g] += l->getUF(e, sf2);
    }
    */
};

#endif // INSTANCE_H
