NAME
   gpprs - GPP OPTIMIZATION

SYNOPSIS
   gpprs [OPTIONS]... -f [FILE]

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

OPTIONS:
   -h, --help     Display this help message.
   -i, --iter     Iterations to perform. Default is 1e6.
   -a, --alpha    Alpha tunning parameter. Default is 1.
   -b, --beta     Beta tunning parameter. Default is 0.1.
   -g, --gamma    Gamma tunning parameter. Default is 1000.
   -m, --method   Optimization method: 0: random, 1: improved random, 2: greedy, 3: GA. Default is 1 (improved random).
   -v, --verbose  Verbose mode. If this option is passed, optimization methods will print progress and intermediate results. Otherwise, only final result is printed.

EXAMPLES:
   1. gpprs -f input.dat
      - Run the program using the default options.

   2. gpprs -f input.dat --iter 10e6
      - Execute 10M iterations.

AUTHORS
   Code was written by Dr. Matias J. Micheletto from CIT-GSJ and supervised by Dr. Rodrigo M. Santos from DIEC-UNS and Dr. Javier Marenco from UTDT.

REPORTING BUGS
   Guidelines will soon be available at <https://github.com/matiasmicheletto/lorawan-optimizations>.

COPYRIGHT
   Copyright   ©   2023   Free   Software   Foundation,  Inc.   License  GPLv3+:  GNU  GPL  version  3  or  later <https://gnu.org/licenses/gpl.html>.
   This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the  extent  permitted by law.
