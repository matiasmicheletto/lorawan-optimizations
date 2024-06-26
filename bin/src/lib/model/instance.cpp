#include "instance.h" 
                        //    7  8  9 10  11  12
const uint Instance::pw[6] = {1, 2, 4, 8, 16, 32}; // Time window values for spread factors

Instance::Instance(char* filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open input file. Check filename or file location." << std::endl;
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

    // Check data consistency
    /*
    std::cout << "Checking instance data consistency..." << std::endl;
    for(uint row = 1; row < edCount; row++){
        for(uint col = 0; col < gwCount; col++){
            const uint sf = this->raw[row][col];
            if(sf < 7 || sf > 12){
                std::cout << "Invalid file, SF values should be in range 7 to 12." << std::endl;
                std::cout << "row " << row << ", col " << col << ", value " << sf << std::endl;
                exit(1);
            }
        }
    }
    */

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
        case FIXED:
            break;
        default:
            std::cerr << "Warning: Instance builder: Invalid period distribution." << std::endl;
            break;
        break;
    }
    CustomDist *periodGenerator = new CustomDist(distBuilder.build());

    this->outputFormat = config.outputFormat;

    // Generate network nodes
    for (uint i = 0; i < config.edNumber; i++) {
        // End device position
        double x, y; 
        posGenerator->setRandom(x, y);        
        // End device period
        int period;
        if(config.timeRequirement == FIXED)
            period = config.fixedPeriod;
        else 
            period = periodGenerator->randomInt(); 
        this->eds.push_back({{x, y}, period});
    }

    for(uint i = 0; i < config.gwNumber; i++) {
        // Gateway position
        double x, y; 
        posGenerator->setRandom(x, y);
        this->gws.push_back({x, y});
    }
    
    // Populate instance matrix with data
    for(uint e = 0; e < this->eds.size(); e++) {
        std::vector<uint> row; // Text line to print
        uint availableGW = 0; // Available GW for this ED
        uint tries = 0;
        while(availableGW == 0 && tries < MAX_TRIES){ // Try many times until feasible system
            for(uint g = 0; g < this->gws.size(); g++) { // Count available GW for this ED
                double dist = euclideanDistance( // Distance from current ED to current GW
                    this->eds[e].pos.x,
                    this->eds[e].pos.y, 
                    this->gws[g].x,
                    this->gws[g].y 
                );
                int minSF = config.scaled ? this->_getMinSFScaled(dist) : this->_getMinSF(dist);
                int maxSF = this->_getMaxSF(this->eds[e].period);            
                if(minSF <= maxSF) // Current ED can be assigned to current GW using at least one SF
                    availableGW++; // Count available gw for this ED
                row.push_back(minSF); // It is only required the minSF because it depends on distance. maxSF can be obtained from period.
            }
            if(availableGW == 0){
                row.clear();
                this->eds[e].period = periodGenerator->randomInt(); // Try other period
                tries++;
            }
        }
        if(tries >= MAX_TRIES){
            std::cerr << "Error: Unfeasible system after many attempts. An End-Device cannot be connected to any Gateway given its period." << std::endl
                        << "ED = " << e << std::endl
                        << "Period = " << this->eds[e].period << std::endl;
            exit(1);
        }
        row.push_back(this->eds[e].period); // Add period as last element
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

void Instance::exportRawData(const char* filename) {

    std::string filenameWithExtension = std::string(filename) + ".dat";
    std::ostream& output = (filename != nullptr) ? *new std::ofstream(filenameWithExtension) : std::cout;


    if (!output) { 
        std::cerr << "Failed to open output stream." << std::endl;
        exit(1);
    }

    for (const auto& row : this->raw) {
        for (int num : row)
            output << num << " ";
        output << '\n';
    }

    if (filename != nullptr) 
        dynamic_cast<std::ofstream&>(output).close();

    std::cout << "Plain text file generated: " << (std::string(filename) + ".dat") << std::endl;
}

void Instance::generateHtmlPlot(const char* filename) {
    // Compute canvas size:
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();

    for (const EndDevice& ed : eds) {
        if (ed.pos.x < minX)
            minX = ed.pos.x;
        if (ed.pos.y < minY)
            minY = ed.pos.y;
        if (ed.pos.x > maxX)
            maxX = ed.pos.x;
        if (ed.pos.y > maxY)
            maxY = ed.pos.y;
    }

    for (const Position& gw : gws) {
        if (gw.x < minX)
            minX = gw.x;
        if (gw.y < minY)
            minY = gw.y;
        if (gw.x > maxX)
            maxX = gw.x;
        if (gw.y > maxY)
            maxY = gw.y;
    }

    // Nodes positions to canvas coordinates
    const double xPadding = 25.0;
    const double yPadding = 25.0;
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;
    const double xScale = (canvasWidth-2*xPadding)/(maxX-minX);
    const double yScale = (canvasHeight-2*yPadding)/(maxY-minY);
    //const int dSize = xScale;
    const int rSize = (xScale+yScale)/4;

    // Calculate canvas size based on the range of coordinates
    //int canvasWidth = static_cast<int>(maxX - minX + 2 * padding);
    //int canvasHeight = static_cast<int>(maxY - minY + 2 * padding);
    
    std::string filenameWithExtension = std::string(filename) + ".html";
    std::ofstream htmlFile(filenameWithExtension);


    if (!htmlFile.is_open()) {
        std::cerr << "Failed to open HTML file." << std::endl;
        return;
    }

    htmlFile << "<!DOCTYPE html>\n";
    htmlFile << "\t<html>\n";
    htmlFile << "\t\t<head>\n";
    htmlFile << "\t\t\t<title>LoRaWAN Position Plot</title>\n";
    htmlFile << "\t\t</head>\n";
    htmlFile << "\t\t<body>\n";
    htmlFile << "\t\t\t<h3>LoRaWAN Generated network</h3>\n";
    htmlFile << "\t\t\t<h5 style='margin-bottom:2px;'>Parameters:</h5>\n";
    htmlFile << "\t\t\t<p style='margin-top:2px;'>GW count = " << this->gwCount << "<br>\n";
    htmlFile << "\t\t\tED count = " << this->edCount << "</p>\n";

    htmlFile << "\t\t\t<canvas id='positionCanvas' width='" 
        << canvasWidth << "' height='" 
        << canvasHeight << "' style='margin-top:20px; margin-left:20px; border: 1px solid black'></canvas>\n";
    htmlFile << "\t\t\t<script>\n";
    htmlFile << "\t\t\t\tvar canvas = document.getElementById('positionCanvas');\n";
    htmlFile << "\t\t\t\tvar ctx = canvas.getContext('2d');\n";

    // Draw positions of End Devices
    htmlFile << "\t\t\t\tctx.fillStyle = 'red';\n";
    htmlFile << "\t\t\t\tctx.beginPath();\n";
    for (const EndDevice& ed : this->eds){
        const double x = (ed.pos.x+maxX)*xScale+xPadding;
        const double y = (ed.pos.y+maxY)*yScale+yPadding;
        htmlFile << "\t\t\t\tctx.moveTo(" << x+rSize << ", " << y << ");\n";
        htmlFile << "\t\t\t\tctx.arc(" << x << ", " << y << ", " << rSize << ", " << "0, Math.PI*2);\n";
        //htmlFile << "\t\t\t\tctx.fillRect(" << x << ", " << y << ", " << dSize << ", " << dSize << ");\n";
    }
    htmlFile << "\t\t\t\tctx.stroke();\n";
    htmlFile << "\t\t\t\tctx.fill();\n";

    // Draw positions of Gateways
    htmlFile << "\t\t\t\tctx.fillStyle = 'blue';\n";
    htmlFile << "\t\t\t\tctx.beginPath();\n";
    for (const Position& gw : this->gws){
        const double x = (gw.x+maxX)*xScale+xPadding;
        const double y = (gw.y+maxY)*yScale+yPadding;
        htmlFile << "\t\t\t\tctx.moveTo(" << x+rSize << ", " << y << ");\n";
        htmlFile << "\t\t\t\tctx.arc(" << x << ", " << y << ", " << rSize*1.5 << ", " << "0, Math.PI*2);\n";
        //htmlFile << "\t\t\t\tctx.fillRect(" << (gw.x+maxX)*xScale << ", " << (gw.y+maxY)*yScale << ", 2, 2);\n";
    }
    htmlFile << "\t\t\t\tctx.stroke();\n";
    htmlFile << "\t\t\t\tctx.fill();\n";

    htmlFile << "\t\t\t</script>\n";
    htmlFile << "\t\t</body>\n";
    htmlFile << "\t</html>\n";

    htmlFile.close();
    std::cout << "HTML file generated: " << (std::string(filename) + ".html") << std::endl;
}

void Instance::generateSvgPlot(const char* filename) {

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();

    for (const EndDevice& ed : eds) {
        if (ed.pos.x < minX)
            minX = ed.pos.x;
        if (ed.pos.y < minY)
            minY = ed.pos.y;
        if (ed.pos.x > maxX)
            maxX = ed.pos.x;
        if (ed.pos.y > maxY)
            maxY = ed.pos.y;
    }

    for (const Position& gw : gws) {
        if (gw.x < minX)
            minX = gw.x;
        if (gw.y < minY)
            minY = gw.y;
        if (gw.x > maxX)
            maxX = gw.x;
        if (gw.y > maxY)
            maxY = gw.y;
    }

    // Nodes positions to canvas coordinates
    const double xPadding = 25.0;
    const double yPadding = 25.0;
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;
    const double xScale = (canvasWidth-2*xPadding)/(maxX-minX);
    const double yScale = (canvasHeight-2*yPadding)/(maxY-minY);
    const int rSize = (xScale+yScale)/3;

    std::string filenameWithExtension = std::string(filename) + ".svg";
    std::ofstream svgFile(filenameWithExtension);


    if (!svgFile.is_open()) {
        std::cerr << "Failed to open SVG file." << std::endl;
        return;
    }
    
    svgFile << "\t\t\t<svg width='" 
        << canvasWidth << "' height='" 
        << canvasHeight << "' style='margin-top:20px; margin-left:20px; border: 1px solid black'>\n"; 

    for (const EndDevice& ed : this->eds){
        const double x = (ed.pos.x+maxX)*xScale+xPadding;
        const double y = (ed.pos.y+maxY)*yScale+yPadding;
        svgFile << "\t\t<circle cx='" << x <<"' cy='" << y << "' r='" << rSize << "' fill='red'/>\n";
    }
    for (const Position& gw : this->gws){
        const double x = (gw.x + maxX)*xScale+xPadding;
        const double y = (gw.y + maxY)*yScale+yPadding;
        svgFile << "\t\t<circle cx='" << x <<"' cy='" << y << "' r='" << rSize*1.3 << "' fill='blue'/>\n";
    }
    svgFile << "</svg>\n";

    svgFile.close();
    std::cout << "SVG image generated: " << (std::string(filename) + ".svg") << std::endl;
}

void Instance::copySFDataTo(std::vector<std::vector<uint>>& destination) {
    // Make a copy of the raw data, only sf values
    copyMatrix(this->raw, destination, 1, this->edCount, 0, this->gwCount-1);
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
    return UtilizationFactor(sf, ufValue);
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

uint Instance::_getMinSFScaled(double distance) {
    if(distance < 2.5)
        return 7;
    else if(distance < 5)
        return 8;
    else if(distance < 10)
        return 9;
    else if(distance < 20)
        return 10;
    else if(distance < 40)
        return 11;
    else if(distance < 80)
        return 12;
    else // No SF for this distance
        return 100;
}

uint Instance::getPeriod(uint ed) {
    return this->raw[ed+1][this->gwCount]; // Last column of raw data
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
        std::cerr << "Error: Unfeasible system. An End-Device cannot be connected to any Gateway given its period." << std::endl
                  << "ED = " << ed << std::endl;
        exit(1);
    }
    return gwList;
}

std::vector<uint> Instance::getSortedGWList(uint ed) {
    // Returns all GW that can be connected to ED sorted by SF (distance)
    std::vector<uint> gwList;
    const uint maxSF = getMaxSF(ed);
    uint frontSF = maxSF;
    //uint backSF = maxSF;
    for(uint gw = 0; gw < this->gwCount; gw++){
        // If SF >= maxSF -> GW is out of range for this ed
        const uint minSF = this->getMinSF(ed, gw);
        if(minSF <= maxSF){ 
            if(minSF < frontSF){
                gwList.insert(gwList.begin(), gw);
                frontSF = minSF; /// TODO: bug
            }else{
                gwList.push_back(gw);
                //backSF = minSF;
            }
        }
    }
    if(gwList.size() == 0) { // No available gws for this ed
        std::cerr << "Error: Unfeasible system. An End-Device cannot be connected to any Gateway given its period." << std::endl
                  << "ED = " << ed << std::endl;
        exit(1);
    }
    return gwList;
}

std::vector<uint> Instance::getSortedGWListByAvailableEd(uint ed) {
    // Returns all GW that can be connected to ED sorted by available ED
    std::vector<uint> gwList;
    std::vector<uint> edListSizes;
    const uint maxSF = getMaxSF(ed);
    for(uint gw = 0; gw < this->gwCount; gw++){
        // If SF >= maxSF -> GW is out of range for this ed
        const uint minSF = this->getMinSF(ed, gw);
        if(minSF <= maxSF){ 
            const uint sz = this->getAllEDList(gw, maxSF).size();
            gwList.push_back(gw); 
            edListSizes.push_back(sz);
        }
    }
    if(gwList.size() == 0) { // No available gws for this ed
        std::cerr << "Error: Unfeasible system. An End-Device cannot be connected to any Gateway given its period." << std::endl
                  << "ED = " << ed << std::endl;
        exit(1);
    }

    std::sort(
        gwList.begin(), 
        gwList.end(), 
        [&edListSizes](const uint &a, const uint &b) {
            return edListSizes[a] < edListSizes[b];
        }
    );

    return gwList;
}

std::vector<uint> Instance::getEDList(uint gw, uint sf) {
    // Returns all ED that can be connected to GW using only the given SF
    std::vector<uint> edList;     
    for(uint ed = 0; ed < this->edCount; ed++){        
        const uint minSF = this->getMinSF(ed, gw);
        const uint maxSF = this->getMaxSF(ed);
        if(minSF <= sf && sf <= maxSF)
            edList.push_back(ed);
    }
    return edList;
}

std::vector<uint> Instance::getAllEDList(uint gw, uint maxSF) {
    // Returns all ED that can be connected to GW using given SF from 7 to maxSF
    std::unordered_set<uint> edSet;
    for(uint s = 7; s < maxSF; s++){
        std::vector<uint> tempList = this->getEDList(gw, s);
        for(uint ee = 0; ee < tempList.size(); ee++)
            edSet.insert(tempList[ee]);
    }
    std::vector<uint> edList(edSet.begin(), edSet.end());
    return edList;
}