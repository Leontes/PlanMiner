#ifndef PRIMITIVETASK_HPP
#define PRIMITIVETASK_HPP

#include "Task/Task.hpp"

/** Class PrimitiveTask.
*  \brief Class to model planning tasks.
*/
class PrimitiveTask : public Task {
private:

public:
  PrimitiveTask():Task(){}
  virtual ~PrimitiveTask ();
};

#endif
