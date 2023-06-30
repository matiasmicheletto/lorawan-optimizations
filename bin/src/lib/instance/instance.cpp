#include "instance.h"

Instance::Instance(char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open input file." << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        int number;
        while (ss >> number)
            row.push_back(number);
        this->raw.push_back(row);
    }
    file.close();

    this->_parseRawData();
}

Instance::Instance(const InstanceConfig& config) {

    // Header of raw data contains number of nodes
    std::vector<int>header = {(int)config.edNumber, (int)config.gwNumber};
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
            printf("Warning: Instance builder: Invalid position distribution");
            break;
    }

    CustomDist::Builder distBuilder = CustomDist::Builder();
    switch (config.timeRequirement){
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
            printf("Warning: Instance builder: Invalid period distribution");
            break;
        break;
    }
    CustomDist *periodGenerator = new CustomDist(distBuilder.build());

    // Generate network nodes
    std::vector<EndDevice> eds;
    std::vector<Position> gws;
    for (unsigned int i = 0; i < config.edNumber; i++) {
        // End device position
        double x, y; 
        posGenerator->setRandom(x, y);        
        // End device period
        int period = periodGenerator->randomInt(); 
        eds.push_back({{x, y}, period});
    }

    for(unsigned int i = 0; i < config.gwNumber; i++) {
        // Gateway position
        double x, y; 
        posGenerator->setRandom(x, y);
        gws.push_back({x, y});
    }
    
    // Populate instance matrix with data
    for(unsigned int i = 0; i < eds.size(); i++){
        std::vector<int> row;
        for(unsigned int j = 0; j < gws.size(); j++){
            double dist = euclideanDistance(
                eds.at(i).pos.x,
                eds.at(i).pos.y, 
                gws.at(j).x,
                gws.at(j).y 
            );
            int minSF = this->_getMinSF(dist);
            row.push_back(minSF);
        }
        row.push_back(eds.at(i).period); // Add period as last element
        this->raw.push_back(row); // Add row to data
    }
    // Raw data is ready to export
}

Instance::~Instance() {

}

void Instance::printRawData() {
    for (const auto& row : this->raw) {
        for (int num : row) 
            std::cout << num << " ";
        std::cout << std::endl;
    }
}

void Instance::_parseRawData() {
    this->edCount = (unsigned int) this->raw[0][0];
    this->gwCount = (unsigned int) this->raw[0][1];
}

unsigned int Instance::getMinSF(unsigned int ed, unsigned int gw){
    return this->raw[ed+1][gw];
}

unsigned int Instance::getMaxSF(unsigned int ed) {
    return this->_getMaxSF(this->getPeriod(ed));
}

double Instance::getUF(unsigned int ed, unsigned int sf) {
    double pw = (double) this->sf2e(sf);
    return pw / ((double)this->getPeriod(ed) - pw);
}

unsigned int Instance::_getMaxSF(unsigned int period) {
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

unsigned int Instance::_getMinSF(double distance) {
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

unsigned int Instance::getPeriod(int ed){
    return this->raw[ed+1][this->gwCount];
}

unsigned int Instance::sf2e(unsigned int sf){
    static const unsigned int arr[6] = {1, 2, 4, 8, 16, 32};
    return arr[sf-7];
}

std::vector<unsigned int> Instance::getGWList(unsigned int ed){
    std::vector<unsigned int> gwList;
    const unsigned int maxSF = getMaxSF(ed);
    for(unsigned int gw = 0; gw < this->gwCount; gw++)
        // If SF >= maxSF -> GW is out of range for this ed
        if(this->getMinSF(ed, gw) <= maxSF) 
            gwList.push_back(gw);
    return gwList;
}