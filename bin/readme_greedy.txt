NAME
   greedy - GREEDY OPTIMIZATION

SYNOPSIS
   greedy [OPTIONS]... -f [FILE]

DESCRIPTION:
   This program computes an optimal solution for the Gateway Placement Problem (GPP). Input file contains the network minimal spread factors values between end devices and gateways formatted as follows:
      - Values are space-separated.
      - First line contains the number of end devices "e" and the number of available gateways "g".
      - From line 1 to "e", each column "c" from 0 to "g"-1 indicate the minimum spread factor that can be used for gateway "c"+1.
      - When a gateway is to far away from an end device, a value greater than 12 is used as value, usually 100.
      - Last column, this is value "g"+1, indicate the transmission period of the corresponding end device.
   Example for the content of a generic input file (10 end-devices and 4 gateways):
      10 4
      7 8 8 10 800
      9 9 7 8 1600
      7 7 10 9 1600
      7 8 8 8 800
      11 9 9 7 800
      7 8 8 10 800
      9 9 7 8 1600
      7 7 10 9 1600
      7 8 8 8 800
      11 9 9 7 800
   The summarized output will be appended to a file under name "summary.csv". Complete output will print to terminal if option "-v" is used when calling the binary.

OPTIONS:
   -h, --help     Display this help message.
   -t, --timeout  Timeout in seconds. Default is 60.  
   -s, --stall    Stagnation threshold (for 10 iterations).  
   -a, --alpha    Alpha tunning parameter. Default is 1.  
   -b, --beta     Beta tunning parameter. Default is 0.01.  
   -g, --gamma    Gamma tunning parameter. Default is 7.8.  
   -w, --wst      Export wst file. 

EXAMPLES:
   1. greedy -f input.dat
      - Run the program using the default options.

   2. greedy -f input.dat --iter 1000 -a 1 -b 0.1 -g 1000
      - Execute 1 thousand iterations using 1, 0.1 and 1000 for alpha, beta and gamma parameters.

   3. greedy -f input.dat -m GA > outputs.log 
      - Run the program using the GA method with default configurations and append complete results to "outputs.log" text file. Text may contain terminal formatting escape characters, so it is recommended to show results using the "cat" program instead of "less".

   4. greedy -f input.dat -m G3 -w solution.wst  
      - Run the Greedy method and export the solution using the wst (xml) format.  

AUTHORS
   Code was written by Dr. Matias J. Micheletto from CIT-GSJ (CONICET) and supervised by Dr. Rodrigo M. Santos from DIEC (UNS) - ICIC (CONICET) and Dr. Javier Marenco from UTDT.

REPORTING BUGS
   Guidelines will soon be available at <https://github.com/matiasmicheletto/lorawan-optimizations>.

COPYRIGHT
   Copyright   ©   2023   Free   Software   Foundation,  Inc.   License  GPLv3+:  GNU  GPL  version  3  or  later <https://gnu.org/licenses/gpl.html>.
   This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the  extent  permitted by law.
