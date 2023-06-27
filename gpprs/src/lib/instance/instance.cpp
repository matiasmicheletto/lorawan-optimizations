#include "instance.h"

using namespace std;

Instance::Instance(char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open input file." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        int number;
        while (ss >> number)
            row.push_back(number);
        this->raw.push_back(row);
    }
    file.close();

    this->_parseRawData();
}

Instance::~Instance() {

}

void Instance::printRawData() {
    for (const auto& row : this->raw) {
        for (int num : row) 
            cout << num << " ";
        cout << endl;
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
    double pw = (double) sf2e(sf);
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

unsigned int Instance::getPeriod(int ed){
    return this->raw[ed+1][this->gwCount];
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