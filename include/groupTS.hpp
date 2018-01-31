#ifndef GROUPTS_HPP
#define GROUPTS_HPP


#include <map>
#include <list>
#include <string>

#include "PlanTrace.hpp"


std::map < std::string, StatesLists > groupTaskStates(std::vector< PlanTrace * > * PTS);



#endif
