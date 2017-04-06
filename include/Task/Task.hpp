#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include <string>
#include <utility>

#include "Task/Parameter.hpp"

/**
* @brief pair <type, value>
*/
typedef std::pair <std::string,std::string> pairParams;


/** Class Task.
*  @brief Class to model planning tasks.
*/
class Task {
protected:
  std::string taskName;
  std::vector<Parameter*> parameters;
public:
  /** Task Constructor
  *   @brief Task class parameter constructor
  *   @param name  Task name
  *   @param iParams  Vector of parameters. Each parameter is a pair<type, value>
  */
  Task(std::string name, std::vector<pairParams> * iParams):taskName(name){
    for(unsigned int i = 0; i < iParams -> size(); i++){
      parameters.push_back(new Parameter("Param_" + std::to_string(i), (*iParams)[i].first, (*iParams)[i].second));
    }
  }

  Task(const Task& task){
    taskName = task.taskName;
    parameters = task.parameters;
  }


  virtual ~Task() {}

  virtual Task * clone(){
    return new Task(*this);
  }

  friend std::ostream& operator << (std::ostream& os, const Task& task){
    os << std::string("( ") << task.taskName << std::string(" ");
    for (unsigned i = 0; i < task.parameters.size(); i++) {
      os << *(task.parameters[i]);
    }

    os << std::string(")\n");
    return os;
  }

};

#endif
