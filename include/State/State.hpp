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
  ~State (){};


  friend std::ostream& operator << (std::ostream& os, const State& state){
    for (unsigned i = 0; i < state.predicates.size(); i++) {
      os << *(state.predicates[i]);
    }
    os << std::endl;
    return  os;
  }
};

#endif
