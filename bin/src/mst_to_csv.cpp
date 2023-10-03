#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <sstream>
#include <string>
#include <iterator>
#include <cstring>

#include <pugixml.hpp>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input_file output_file" << std::endl;
        return 1;
    }

    const char* inputFileName = argv[1];
    const char* outputFileName = argv[2];

    pugi::xml_document doc;
    if (!doc.load_file(inputFileName)) {
        std::cerr << "Failed to load XML file: " << inputFileName << std::endl;
        return 1;
    }

    std::ofstream csvFile(outputFileName, std::ofstream::out);

    std::regex numberPattern("\\d+");

    pugi::xpath_node_set variableNodes = doc.select_nodes(".//variable");

    for (pugi::xpath_node variableNode : variableNodes) {
        const char* name = variableNode.node().attribute("name").as_string();
        if (name && std::strncmp(name, "w", 1) == 0) {
            continue;
        }

        std::string nameStr(name);
        std::smatch matches;
        std::vector<std::string> numbers;

        while (std::regex_search(nameStr, matches, numberPattern)) {
            numbers.push_back(matches.str());
            nameStr = matches.suffix();
        }

        if (!numbers.empty()) {
            std::ostringstream rowStream;
            std::copy(numbers.begin(), numbers.end() - 1, std::ostream_iterator<std::string>(rowStream, ","));
            rowStream << numbers.back() << std::endl;
            csvFile << rowStream.str();
        }
    }

    csvFile.close();

    return 0;
}
