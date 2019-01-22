#ifndef PLANTRACE_HPP
#define PLANTRACE_HPP

#include <vector>
#include <list>
#include "StateLib.hpp"

typedef std::pair<std::list<State*>, std::list<State*> > StatesLists;


/** Class PlanTrace.
*  @brief Class to model planning tasks.
*/
class PlanTrace {
private:
  std::vector<TSLinker * > TSlinks;

public:
  PlanTrace(){};
  ~PlanTrace(){};

  /** Function addLink
  *   @brief Method to add a TaskStateLinker to the plan trace
  *   @param link TaskStateLinker to add
  */
  void addLink(TSLinker * link){
    TSlinks.push_back(link);
  }

  /** Function getNTS
  *   @brief Method to get the number of TaskStateLinker of the PlanTrace object
  *   @retval the number of TaskStateLinker objects
  */
  unsigned int getNTS(){
    return TSlinks.size();
  }

  /** Function getNTS
  *   @brief Method to get a certain TaskStateLinker object
  *   @param i integer TaskStateLinker to obtain
  *   @retval TSLinker TaskStateLinker object
  */
  TSLinker * getTS(unsigned int i){
    return TSlinks[i];
  }

  /** Function stateLenght
  *   @brief Method to mean number of predicates of the PlanTrace's states
  *   @retval double mean number of predicates
  */
  double stateLenght(){
    double sLength = 0.0;
    for (unsigned int i = 0; i < TSlinks.size(); i++){
      sLength += (TSlinks[i]) -> stateLenght();
    }
    return (double)sLength/(double)TSlinks.size();
  }

  /** Function operator <<
  *   @brief Method to output the PlanTrace object
  *   @retval ostream PlanTrace object info
  */
  friend std::ostream& operator << (std::ostream& os, const PlanTrace& pt){
    os << "New Plan!!" << std::endl << std::endl;
    for (unsigned int i = 0; i < pt.TSlinks.size(); i++) {
      os << "TSLink: " << i << std::endl;
      os << *(pt.TSlinks[i]) << std::endl;
    }
    return os;
  }

  /** Method ParseTask
  *   @brief Method to calculate the schema form of every Task/State of the plan trace.
  */
  void schematize(/*std::vector < Type *> tipos*/);

};

#endif
