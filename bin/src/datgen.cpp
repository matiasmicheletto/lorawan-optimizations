#define MANUAL "readme_datgen.txt"

#include <cstring>
#include "lib/util/util.h"
#include "lib/model/instance.h"


int main(int argc, char **argv) {
    
    srand(time(NULL));

    InstanceConfig config; // Default configuration

    char* outputFileName = nullptr;

    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1)
            printHelp(MANUAL);
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i+1 < argc) {
                size_t len = strlen(argv[i+1]);
                outputFileName = new char[len+1];
                strcpy(outputFileName, argv[i+1]);
            }else{
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
            if(i+1 < argc){
                config.outputFormat = (std::strcmp(argv[i+1], "HTML") == 0) ? OUTFORMAT::HTML : OUTFORMAT::TXT;
            }else
                printHelp(MANUAL);
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
            if(i+1 < argc){
                config.timeRequirement = (PERIODIST) atoi(argv[i+1]);
                if(config.timeRequirement == FIXED){
                    config.fixedPeriod = atoi(argv[i+2]);
                }
            }else{
                printHelp(MANUAL);
            }
        }
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--positions") == 0) {
            if(i+1 < argc)
                config.posDistribution = (POSDIST) atoi(argv[i+1]);
            else
                printHelp(MANUAL);
        }
    }

    Instance* l = new Instance(config);

    if(config.outputFormat == OUTFORMAT::TXT)
        l->exportRawData(outputFileName);
    if(config.outputFormat == OUTFORMAT::HTML)
        l->generateHtmlPlot(outputFileName);
    
    delete(l);
    l = 0;
    
    return 0;
}
