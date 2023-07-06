#define MANUAL "readme_datgen.txt"

#include <cstring>
#include "lib/util/util.h"
#include "lib/model/instance.h"


int main(int argc, char **argv) {
    
    srand(time(NULL));

    InstanceConfig config; // Default configuration
    bool output = false; // Print data to output file
    char* outputFileName;

    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i+1 < argc) {
                output = true;
                size_t len = strlen(argv[i+1]);
                outputFileName = new char[len+1];
                strcpy(outputFileName, argv[i+1]);
            }else{
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--enddevices") == 0) {
            if(i+1 < argc)
                config.edNumber = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gateways") == 0) {
            if(i+1 < argc)
                config.gwNumber = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--map") == 0) {
            if(i+1 < argc)
                config.mapSize = atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--requirements") == 0) {
            if(i+1 < argc)
                config.timeRequirement = (PERIODIST) atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--positions") == 0) {
            if(i+1 < argc)
                config.posDistribution = (POSDIST) atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
    }

    Instance* l = new Instance(config);

    if(output)
        l->exportRawData(outputFileName);
    else
        l->printRawData();

    delete(l);
    l = 0;
    
    return 0;
}
