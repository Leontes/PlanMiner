#ifndef STATE_HPP
#define STATE_HPP

#include <vector>

#include "State/Predicate.hpp"


/** Class State.
*  \brief Class to model logic states.
*/
class State {
private:
  std::vector<Predicate *> predicates;

public:
  State ();
  virtual ~State ();
};

#endif
