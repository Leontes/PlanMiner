#ifndef TYPESHIER_HPP
#define TYPESHIER_HPP


#include <vector>
#include <map>

#include "PlanTrace.hpp"





std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS, std::map <std::string, std::vector <std::pair <std::string, std::string> > > * mapaPrimitivasParam);



#endif
