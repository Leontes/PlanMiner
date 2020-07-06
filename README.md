# PlanMiner
PlanMiner domain learner algorithm Github repository. 

General questions and contact: José Á. Segura-Muros <josesegmur@decasi.ugr.es>

Test folder contains the experimental data to test the algorithm. Each folder contains a planning domain used in the experimental process and the source code of an executable generator of domain's problems. plantrace.txt is an example Plan Trace ready to use as the input of PlanMiner. Scripts folder contains two scripts to include noise and incompleteness in a Plan Trace.

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
