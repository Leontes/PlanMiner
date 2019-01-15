#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <string>

#include "State/Conector.hpp"
#include "StateLib.hpp"



/** Class State.
*  @brief Class to model logic states.
*/
class State {
private:
  std::vector<Predicate *> predicates;

public:
  /** State Constructor
  *   @brief State class parameter constructor
  *   @param tokens  list of string preparsed tokens
  */
  State(std::vector<std::string> tokens);
  State(const State & iState){
    for (unsigned int i = 0; i < iState.predicates.size(); i++) {
      predicates.push_back(iState.predicates[i] -> clone());
    }

  }
  ~State (){};


  void clearParam(std::string newValue, std::string value, std::vector<std::string> types);

  void clean();

  friend std::ostream& operator << (std::ostream& os, const State& state){
    for (unsigned i = 0; i < state.predicates.size(); i++) {
      os << *(state.predicates[i]);
    }
    os << std::endl;
    return  os;
  }

  unsigned int length(){
    unsigned int sLength = 0;
    for (unsigned i = 0; i < predicates.size(); i++) {
      sLength += predicates[i] -> length();
    }
    return sLength;
  }

  void to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index);

  double getCost(std::string pred);
  void setCost(std::string pred, double cost);

  std::vector<std::string> getAllFunctions();

};

#endif
