#ifndef PLANTRACE_HPP
#define PLANTRACE_HPP

#include <vector>
#include <list>
#include "TSLinker.hpp"

typedef std::pair<std::list<State*>, std::list<State*> > StatesLists;


/** Class PlanTrace.
*  \brief Class to model planning tasks.
*/
class PlanTrace {
private:
  std::vector<TSLinker * > TSlinks;

public:
  PlanTrace(){};
  ~PlanTrace(){};

  void addLink(TSLinker * link){
    TSlinks.push_back(link);
  }

  unsigned int getNTS(){
    return TSlinks.size();
  }

  TSLinker * getTS(unsigned int i){
    return TSlinks[i];
  }

  unsigned int lenght(){
    return TSlinks.size();
  }

  double stateLenght(){
    double sLength = 0.0;
    for (unsigned int i = 0; i < TSlinks.size(); i++){
      sLength += (TSlinks[i]) -> stateLenght();
    }
    return (double)sLength/(double)TSlinks.size();
  }


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
