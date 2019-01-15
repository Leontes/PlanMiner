#!/bin/bash


mkdir Dominios

./bin/PlanMiner-O2 Problemas/BlocksWorld/pts/BlocksWorld.pts BlocksWorld
cp BlocksWorld.pddl Dominios/BlocksWorld.pddl



./bin/PlanMiner-O2 Problemas/Depots/pts/Depots.pts Depots
cp Depots.pddl Dominios/Depots.pddl



./bin/PlanMiner-O2 Problemas/DriverLog/pts/DriverLog.pts DriverLog
cp DriverLog.pddl Dominios/DriverLog.pddl



./bin/PlanMiner-O2 Problemas/Satellite/pts/Satellite.pts Satellite
cp Satellite.pddl Dominios/Satellite.pddl



./bin/PlanMiner-O2 Problemas/Zenotravel/pts/Zenotravel.pts Zenotravel
cp Zenotravel.pddl Dominios/Zenotravel.pddl
