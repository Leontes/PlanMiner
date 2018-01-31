#ifndef FUSERULES_HPP
#define FUSERULES_HPP

#include <vector>
#include <list>
#include <string>
#include <utility>


void fuseRules(std::vector < std::pair <int, std::list< std::pair<std::string, std::string> > > > * reglasInp,
  std::vector <std::list< std::pair<std::string, std::string> > > * reglasOut,
  std::vector < std::vector < double > > * dataset,
  std::vector < std::string > * attribLabels);


#endif
