# PlanMiner
PlanMiner is a domain learner algorithm. 

## Features
- Learning planning domains from incomplete Plan Traces.
- Automatic discovering of arithmetic and logic expressions from the input data.

Test folder contains example several Plan Traces to try out the domain learning algorithm.
## Supported Platforms:
- Linux

## Dependencies needed to compile the software:
- g++
- cmake
- make

## Installation and usage:
```
cmake .
make
./bin/PlanMiner ptFilename dFilename
```

ptFilename is the path and name of the input plan trace. dFilename is the output domain's name.
