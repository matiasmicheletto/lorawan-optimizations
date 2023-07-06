#include "instance.h"

Instance::Instance(char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open input file." << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<uint> row;
        std::stringstream ss(line);
        int number;
        while (ss >> number)
            row.push_back(number);
        this->raw.push_back(row);
    }
    file.close();

    this->edCount = this->raw[0][0];
    this->gwCount = this->raw[0][1];
    this->instanceFileName = new char[strlen(filename) + 1];
    strcpy(this->instanceFileName, extractFileName(filename));
}

Instance::Instance(const InstanceConfig& config) {

    // Header of raw data contains number of nodes
    std::vector<uint>header = {config.edNumber, config.gwNumber};
    this->raw.push_back(header);
    
    // Create the random generator functions
    Random* posGenerator;
    switch (config.posDistribution) {
        case UNIFORM: // Positions have a uniform distribution
            posGenerator = new Uniform(-(double)config.mapSize/2, (double)config.mapSize/2);
            break;
        case NORMAL: // Positions have normal distribution
            posGenerator = new Normal(-(double)config.mapSize/2, (double)config.mapSize/2);
            break;
        case CLOUDS:{ // Positions fall with a uniform distribution of 3 normal distributions 
            posGenerator = new Clouds(-(double)config.mapSize/2, (double)config.mapSize/2, 5);
            break;
        }
        default:
            std::cerr << "Warning: Instance builder: Invalid position distribution." << std::endl;
            break;
    }

    CustomDist::Builder distBuilder = CustomDist::Builder();
    switch (config.timeRequirement) {
        case SOFT:
            distBuilder.addValue(16000, 0.25)
                ->addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(3200, 0.25);

            break;
        case MEDIUM:
		    distBuilder.addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(2000, 0.25)
                ->addValue(1600, 0.25);
            break;
        case HARD:
            distBuilder.addValue(1600, 0.25)
                ->addValue(800, 0.25)
                ->addValue(400, 0.25)
                ->addValue(320, 0.25);
            break;
        default:
            std::cerr << "Warning: Instance builder: Invalid period distribution." << std::endl;
            break;
        break;
    }
    CustomDist *periodGenerator = new CustomDist(distBuilder.build());

    // Generate network nodes
    std::vector<EndDevice> eds;
    std::vector<Position> gws;
    for (uint i = 0; i < config.edNumber; i++) {
        // End device position
        double x, y; 
        posGenerator->setRandom(x, y);        
        // End device period
        int period = periodGenerator->randomInt(); 
        eds.push_back({{x, y}, period});
    }

    for(uint i = 0; i < config.gwNumber; i++) {
        // Gateway position
        double x, y; 
        posGenerator->setRandom(x, y);
        gws.push_back({x, y});
    }
    
    // Populate instance matrix with data
    for(uint i = 0; i < eds.size(); i++) {
        std::vector<uint> row;
        uint availableGW = 0;
        for(uint j = 0; j < gws.size(); j++) {
            double dist = euclideanDistance(
                eds.at(i).pos.x,
                eds.at(i).pos.y, 
                gws.at(j).x,
                gws.at(j).y 
            );
            int minSF = this->_getMinSF(dist);
            int maxSF = this->_getMaxSF(eds.at(i).period);
            if(minSF <= maxSF)
                availableGW++; // Count available gw for this ED
            row.push_back(minSF);
        }
        if(availableGW == 0) {
            std::cerr << "Error: Unfeasible system. An End-Device cannot be allocated to any Gateway given its period." << std::endl
                        << "ED = " << i << std::endl
                        << "Period = " << eds.at(i).period << std::endl;
            exit(1);
        }
        row.push_back(eds.at(i).period); // Add period as last element
        this->raw.push_back(row); // Add row to data
    } // Raw data is ready to export (or use)
    // Set attributes (in case of using config to generate an instance to solve)
    this->edCount = config.edNumber;
    this->gwCount = config.gwNumber;
}

Instance::~Instance() {
    delete[] this->instanceFileName;
}

void Instance::printRawData() {
    for (const auto& row : this->raw) {
        for (int num : row) 
            std::cout << num << " ";
        std::cout << std::endl;
    }
}

void Instance::exportRawData(char* filename) {
    std::ofstream outputFile(filename);
    
    if (!outputFile.is_open()) { 
        std::cerr << "Failed to open output file." << std::endl;
        exit(1);
    }
    
    for (const auto& row : this->raw) {
        for (int num : row)
            outputFile << num << " ";
        outputFile << '\n';
    }

    outputFile.close();
}

void Instance::copySFDataTo(std::vector<std::vector<uint>>& destination) {
    // Make a copy of the raw data, only sf values
    copyMatrix(this->raw, destination, 1, this->getEDCount(), 0, this->getGWCount()-1);
}

uint Instance::getMinSF(uint ed, uint gw) {
    return this->raw[ed+1][gw];
}

uint Instance::getMaxSF(uint ed) {
    return this->_getMaxSF(this->getPeriod(ed));
}

UtilizationFactor Instance::getUF(uint ed, uint sf) {
    double pw = (double) this->sf2e(sf);
    double ufValue = pw / ((double)this->getPeriod(ed) - pw);
    UtilizationFactor uf(sf, ufValue);
    return uf;
}

uint Instance::_getMaxSF(uint period) {
    // max Sf = (log(T)-2)/log(2) + 7;

    if(period >= 3200)
        return 12;
    else if(period >= 1600)
        return 11;
    else if(period >= 800)
        return 10;
    else if(period >= 400)
        return 9;
    else if(period >= 200)
        return 8;
    else if(period >= 100)
        return 7;
    else // No SF for this period
        return 0;
}

uint Instance::_getMinSF(double distance) {
    if(distance < 62.5)
        return 7;
    else if(distance < 125)
        return 8;
    else if(distance < 250)
        return 9;
    else if(distance < 500)
        return 10;
    else if(distance < 1000)
        return 11;
    else if(distance < 2000)
        return 12;
    else // No SF for this distance
        return 100;
}

uint Instance::getPeriod(int ed) {
    return this->raw[ed+1][this->gwCount]; // Last column of raw data
}

uint Instance::sf2e(uint sf) {
    static const uint arr[6] = {1, 2, 4, 8, 16, 32};
    return arr[sf-7];
}

std::vector<uint> Instance::getGWList(uint ed) {
    // Returns all GW that can be connected to ED
    std::vector<uint> gwList;
    const uint maxSF = getMaxSF(ed);
    for(uint gw = 0; gw < this->gwCount; gw++){
        // If SF >= maxSF -> GW is out of range for this ed
        const uint minSF = this->getMinSF(ed, gw);
        if(minSF <= maxSF) 
            gwList.push_back(gw);
    }
    if(gwList.size() == 0) { // No available gws for this ed
        std::cerr << "Error: Unfeasible system. An End-Device cannot be allocated to any Gateway given its period." << std::endl
                  << "ED = " << ed << std::endl;
        exit(1);
    }
    return gwList;
}
