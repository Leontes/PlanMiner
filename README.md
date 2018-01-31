# PlanMiner-O2
PlanMiner-O2 is a domain learner algorithm. 

General questions and contact: José Á. Segura-Muros <josesegmur@decasi.ugr.es>

Data folder contains the experimental data used in the paper sent to the IJCAI 2018. Each folder contains a planning domain used in the experimental process and the source code of an executable generator of domain's problems. plantrace.txt is an example Plan Trace ready to use as the input of PlanMiner-O2. Scripts folder contains two scripts to include noise and incompleteness in a Plan Trace.

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

## Scripts usage:
```
	python addNoise.py ptFilename nVal
	python addIncompleteness.py ptFilename nVal
```

ptFilename is the path and name of the input plan trace.
nVal is the percentage of fluents modified by the script. Is a numeric value between [0, 1]
