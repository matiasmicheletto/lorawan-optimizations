#define MANUAL "readme_datgen.txt"

#include <cstring>

#include "lib/util/util.h"
#include "lib/instance/instance.h"


int main(int argc, char **argv) {
    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            printHelp(MANUAL);
    }

    Instance* l = new Instance();

    l->printRawData();

    delete(l);
    l = 0;
    
    return 0;
}
