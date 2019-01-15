#ifndef FUSERULES_HPP
#define FUSERULES_HPP

#include <vector>
#include <map>
#include <list>
#include <string>
#include <utility>


void fuseRules(std::vector < std::pair <int, std::list< std::pair<std::string, std::string> > > > * reglasInp,
  std::vector <std::list< std::pair<std::string, std::string> > > * reglasOut,
    std::vector < std::vector < double > > * dataset,
      std::vector < std::pair <std::string, std::string> > attribLabels,
        std::vector <std::pair <std::string, std::string> > * cabeceras);


#endif
