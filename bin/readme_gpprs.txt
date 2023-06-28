NAME
   gpprs - GPP OPTIMIZATION

SYNOPSIS
   gpprs [OPTIONS]... -f [FILE]

DESCRIPTION:
   This program computes an optimal solution for the Gateway Placement Problem (GPP).


OPTIONS:
   -h, --help     Display this help message.
   -i, --iter     Iterations to perform. Default is 1e6.
   --alpha        Alpha tunning parameter. Default is 1.
   --beta         Beta tunning parameter. Default is 1.
   --gamma        Gamma tunning parameter. Default is 1.

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
