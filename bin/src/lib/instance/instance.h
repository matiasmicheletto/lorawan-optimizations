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

#include "../util/util.h"
#include "../random/random.h"
#include "../random/uniform.h"
#include "../random/normal.h"
#include "../random/clouds.h"
#include "../random/custom.h"

enum POSDIST {UNIFORM, NORMAL, CLOUDS};
enum PERIODIST {SOFT, MEDIUM, HARD};

struct InstanceConfig { // This is for instance generation
    uint mapSize; // Map size
    uint edNumber; // Number of end devices (rows)
    uint gwNumber; // Number of gateways (columns)
    PERIODIST timeRequirement; // PDF for ED periods
    POSDIST posDistribution; // Distribution of positions

    InstanceConfig(  // Default configuration parameters
        uint mapSize = 1000,
        uint edNumber = 1000,
        uint gwNumber = 100,
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
        void exportRawData(char* filename);
        void copySFDataTo(std::vector<std::vector<uint>>& destination);
        
        inline uint getGWCount(){return this->gwCount;}
        inline uint getEDCount(){return this->edCount;}
        inline char* getInstanceFileName(){return this->instanceFileName;}
        uint getMinSF(uint ed, uint gw);
        uint getMaxSF(uint ed);
        double getUF(uint ed, uint sf);
        uint getPeriod(int ed);
        uint sf2e(uint sf);
        std::vector<uint> getGWList(uint ed);

    private:
        std::vector<std::vector<uint>> raw;
        uint gwCount, edCount;
        char* instanceFileName;

        uint _getMaxSF(uint period);
        uint _getMinSF(double distance);
};

#endif // INSTANCE_H
