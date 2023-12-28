NAME
   datgen - Instance generator for GPP

SYNOPSIS
   datgen [OPTIONS]... 

DESCRIPTION:
   This program generates instances for the Gateway Placement Problem (GPP), to be solved using the optimization program. Depending on the distribution function configured for node positioning, a random network of end devices and gateways is generated. Each end device is assigned a period depending on the configured real time requirements and finally a matrix is printed with one row for each end device and one column for each gateway. The final column indicates the transmission period of each end device.

OPTIONS:
   -h, --help           Display this help message.  
   -e, --enddevices     Number of end-devices. Default is 1000.  
   -g, --gateways       Number of gateways. Default is 100.  
   -r, --requirements   Real-time requirements for end-devices: 0=soft, 1=intermediate, 2=hard, 3=fixed (add value after the 3). Default is soft.  
   -p, --positions      Placement of nodes: 0=uniform, 1=normal, 2=clouds. Default is uniform. Gateways possible positions are generated with this function too.  
   -m, --map            Map size. Default is 1000.  
   -o, --oputput        Output file name.  
   -f, --format         Output file format: TXT, HTML (for plot ED and GW distribution), XML (not implemented yet).  

EXAMPLES:
   1. datgen 
      - Prints this manual
   2. datgen [...args] > instance.dat 
      - Generates an instance file with name "instance.dat"
   3. datget [...args] -o instance.dat
      - Prints raw data to file with name "instance.dat"
   4. datgen -m 100 -e 1000 -g 50 -r 1 -p 2
      - Generate the instance for a network of 1050 nodes (1000 end devices and 50 gateways) with cloud distribution of positions over a 100x100 map. Time requirements are intermediate.
   5. datgen -m 100 -e 5000 -g 10 -r 3 1600 -p 2
      - Generate a similar instance as the previous example and using a fixed value for periods (1600).  

AUTHORS
   Code was written by Dr. Matias J. Micheletto from CIT-GSJ and supervised by Dr. Rodrigo M. Santos from DIEC-UNS and Dr. Javier Marenco from UTDT.

REPORTING BUGS
   Guidelines will soon be available at <https://github.com/matiasmicheletto/lorawan-optimizations>.

COPYRIGHT
   Copyright   ©   2023   Free   Software   Foundation,  Inc.   License  GPLv3+:  GNU  GPL  version  3  or  later <https://gnu.org/licenses/gpl.html>.
   This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the  extent  permitted by law.
