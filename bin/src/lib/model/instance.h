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
enum OUTPUTFORMAT {NONE, TXT, HTML};

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
    std::vector<bool> allocated;
    std::vector<UtilizationFactor> uf;
    Instance *l;

	Allocation(Instance* l) {
		gw.resize(l->edCount);
		sf.resize(l->edCount);
        allocated.resize(l->edCount);
        uf.resize(l->gwCount);
        this->l = l;
	}

    void connect(uint e, uint g, int asf = -1) {
        const uint sf2 = (asf == -1 ? l->getMinSF(e, g) : asf); // Use provided or min SF as default
        gw[e] = g;
        sf[e] = sf2;
        allocated[e] = true;
        uf[g] += l->getUF(e, sf2);
    }

    bool checkBeforeConnect(uint e, uint g, int asf = -1) {
        const uint sf2 = (asf == -1 ? l->getMinSF(e, g) : asf); // Use provided or min SF as default
        const UtilizationFactor nextUF = l->getUF(e, sf2); // UF of node e for g
        if((uf[g] + nextUF).isFull()) // If g not available for e with sf2, return
            return false;
        // else, allocate and return
        gw[e] = g;
        sf[e] = sf2;
        allocated[e] = true;
        uf[g] += nextUF;
        return true;
    }
};

#endif // INSTANCE_H
