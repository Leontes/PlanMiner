#ifndef PLANTRACE_HPP
#define PLANTRACE_HPP

#include <vector>

#include "TSLinker.hpp"

/** Class PlanTrace.
*  \brief Class to model planning tasks.
*/
class PlanTrace {
private:
  std::vector<TSLinker> tasks;

public:
  PlanTrace ();
  virtual ~PlanTrace ();
};

#endif
