#ifndef TOTABLE_HPP
#define TOTABLE_HPP


#include <string>

#include "PlanTrace.hpp"




void to_table( std::string task, StatesLists states, std::vector < std::vector < std::vector < double > > > * datasets, std::vector < std::vector < std::pair<std::string, std::string> > > * attribLabelsVC);



#endif
