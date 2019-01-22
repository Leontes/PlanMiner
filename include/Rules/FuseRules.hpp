#ifndef FUSERULES_HPP
#define FUSERULES_HPP

#include <vector>
#include <map>
#include <list>
#include <string>
#include <utility>

/** Method fuseRules
*   @brief Method to fuse a various sets of rules in a only unique set
*   @param reglasInp STD vector with the Input rules
*   @param reglasOut STD vector with the Output rules resultant of the fusing process
*   @param dataset pair of STD matrices of double values
*   @param attribLabels STD vector with the labels of the datasets' columns
*   @param cabeceras STD vector with the headers of a planning task
*/
void fuseRules(std::vector < std::pair <int, std::list< std::pair<std::string, std::string> > > > * reglasInp,
  std::vector <std::list< std::pair<std::string, std::string> > > * reglasOut,
    std::vector < std::vector < double > > * dataset,
      std::vector < std::pair <std::string, std::string> > attribLabels,
        std::vector <std::pair <std::string, std::string> > * cabeceras);


#endif
