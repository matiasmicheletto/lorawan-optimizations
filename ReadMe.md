# Real-time LoRaWAN Simulations & Optimization

Given the model of a LoRaWAN network with end devices nodes and gateways, this software determines the best allocation of nodes to gateways and spread factors. Different optimization algorithms are proposed: from random search and greedy methods to ILP and evolutionary approaches.

Authors:
  * Dr. Rodrigo Santos from DIEC UNS, ICIC CONICET-UNS
  * Dr. Javier Marenco from UTDT
  * Dr. Matías Micheletto from IIDEPYS CONICET-UNPSJB


## Usage

#### Compile
```bash
cd bin
make
```

#### Create instance file with network model
```bash
# Print binary manual:
datgen 

# Generate the instance file for a network of 1050 nodes (1000 end devices and 50 gateways) with cloud distribution of positions over a 100x100 map. Time requirements are intermediate.
datgen -m 100 -e 1000 -g 50 -r 1 -p 2 > instance.dat 
```

#### Solve instance

```bash
# Print binary manual
gpprs

# Solve instance using Greedy method with (1, 0.01, 7.8) as tunning parameters, 100 iterations, 30 seconds timeout and verbose mode on. Export output to outputs.log file.
gpprs -f instance.dat -a 1 -b 0.01 -g 7.8 -i 100 -t 30 -m GD -v >> outputs.log 
```