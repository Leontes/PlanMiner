#ifndef TSLINKER_HPP
#define TSLINKER_HPP

#include <stdio.h>
#include <string.h>


#include "State/State.hpp"
#include "TaskLib.hpp"

struct Type{
  Type * parent;
  std::string name;

  Type(std::string n){
    name = n;
    parent = 0;
  }

  bool isType(std::string typeName){
    if(name == typeName){
      return true;
    }
    else{
      if(parent != 0){
        return (*parent).isType(typeName);
      }
      else{
        if(typeName == "OBJECT"){
          return true;
        }
        else{
          return false;
        }

      }
    }
  }

  void addParent(Type * p){
    parent = p;
  }

  std::vector<std::string> getTypeHierarchy(){
    std::vector<std::string> out;
    if(parent != 0){
      out = parent -> getTypeHierarchy();
    }
    else{
      out.push_back("OBJECT");
    }
    out.push_back(name);

    return out;
  }
};


/** Class TSLinker.
*  \brief Class to model planning tasks.
*/
class TSLinker {
private:
  State * preState;
  State * postState;
  Task * task;

public:
  TSLinker (State * iPreState, State * iPostState, Task * iTask){
    if(iPreState != nullptr){
      preState = new State(*iPreState);
    }
    else{
      preState = nullptr;
    }

    if(iPostState != nullptr){
      postState = new State(*iPostState);
    }
    else{
      postState = nullptr;
    }

    task = new Task(*iTask);
  }
  ~TSLinker(){
    delete preState;
    delete postState;
    delete task;
  }

  /** Function getTask
  *   @brief Method to get the TSLinker object's task
  *   @retval Task object
  */
  Task * getTask(){
    return task;
  }

  /** Function getPreS
  *   @brief Method to get the TSLinker object's pre-state
  *   @retval State object
  */
  State * getPreS(){
    return preState;
  }

  /** Function getPostS
  *   @brief Method to get the TSLinker object's post-state
  *   @retval State object
  */
  State * getPostS(){
    return postState;
  }

  /** Function stateLenght
  *   @brief Method to get the lenght of the TSLinker object's states
  *   @retval double states length sum
  */
  double stateLenght(){
    unsigned int sLength = 0, n = 0;
    if(preState != nullptr){
      sLength += preState -> length();
      n++;
    }
    if(postState != nullptr){
      sLength += postState -> length();
      n++;
    }

    if(n == 0){
      return 0.0;
    }else{
      return (double)sLength/(double)n;
    }
  }

  /** Function operator <<
  *   @brief Method to output the TSLinker object
  *   @retval ostream TSLinker object info
  */
  friend std::ostream& operator << (std::ostream& os, const TSLinker& ts){
    if(ts.preState != nullptr){
      os << "PreState: \n" << *(ts.preState) << std::endl;
    }else{
      os << "PreState: Missing State" << std::endl;
    }

    if(ts.postState != nullptr){
      os << "PostState: \n" << *(ts.postState) << std::endl;
    }else{
      os << "PostState: Missing State" << std::endl;
    }

    if(ts.task != nullptr){
      os << "Task: \n" << *(ts.task) << std::endl;
    }else{
      os << "Task: Missing Task" << std::endl;
    }
    return os;
  }

  /** Function schematize
  *   @brief Method to put the tasks and states of the TSLinker object in it into its schema form
  */
  void schematize(/*std::vector < Type *> tipos*/);
};

#endif
