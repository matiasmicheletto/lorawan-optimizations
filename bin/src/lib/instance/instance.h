#ifndef INSTANCE_H
#define INSTANCE_H

/* 
    Class Instance: Models problem parameters. Load parameters from input file 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "../util/util.h"
#include "../random/random.h"
#include "../random/uniform.h"
#include "../random/normal.h"
#include "../random/clouds.h"
#include "../random/custom.h"

enum POSDIST {UNIFORM, NORMAL, CLOUDS};
enum PERIODIST {SOFT, MEDIUM, HARD};

struct InstanceConfig { // This is for instance generation
    unsigned int mapSize; // Map size
    unsigned int edNumber; // Number of end devices (rows)
    unsigned int gwNumber; // Number of gateways (columns)
    PERIODIST timeRequirement; // PDF for ED periods
    POSDIST posDistribution; // Distribution of positions

    InstanceConfig(  // Default configuration parameters
        unsigned int mapSize = 1000,
        unsigned int edNumber = 1000,
        unsigned int gwNumber = 100,
        PERIODIST timeRequirement = SOFT,
        POSDIST posDistribution = UNIFORM
    ) : 
        mapSize(mapSize),
        edNumber(edNumber),
        gwNumber(gwNumber),
        timeRequirement(timeRequirement),
        posDistribution(posDistribution) {}
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
        
        inline unsigned int getGWCount(){return this->gwCount;}
        inline unsigned int getEDCount(){return this->edCount;}
        unsigned int getMinSF(unsigned int ed, unsigned int gw);
        unsigned int getMaxSF(unsigned int ed);
        double getUF(unsigned int ed, unsigned int sf);
        unsigned int getPeriod(int ed);
        std::vector<unsigned int> getGWList(unsigned int ed);

    private:
        std::vector<std::vector<int>> raw;
        unsigned int gwCount, edCount;

        void _parseRawData();
        unsigned int _getMaxSF(unsigned int period);
        unsigned int _getMinSF(double distance);
};

#endif // INSTANCE_H
