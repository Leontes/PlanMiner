#ifndef TSLINKER_HPP
#define TSLINKER_HPP

#include "StateLib.hpp"
#include "TaskLib.hpp"

/** Class TSLinker.
*  \brief Class to model planning tasks.
*/
class TSLinker {
private:
  State preState;
  State postState;
  Task task;

public:
  TSLinker ();
  virtual ~TSLinker ();
};

#endif
