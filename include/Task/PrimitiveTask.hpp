#ifndef PRIMITIVETASK_HPP
#define PRIMITIVETASK_HPP

#include "Task/Task.hpp"

/** Class PrimitiveTask.
*  \brief Class to model planning tasks.
*/
class PrimitiveTask : public Task {
private:

public:
  PrimitiveTask(std::string name, std::vector<pairParams> * iParams):Task(name, iParams){}
  ~PrimitiveTask(){}


  friend std::ostream& operator << (std::ostream& os, const PrimitiveTask& task){
    os << std::string("( ") << task.taskName << std::string(" ");
    for (unsigned i = 0; i < task.parameters.size(); i++) {
      os << *(task.parameters[i]);
    }

    os << std::string(")\n");
  }

};

#endif
