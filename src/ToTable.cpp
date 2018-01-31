#include "ToTable.hpp"





void to_table( std::string task, StatesLists states, std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels){

  unsigned int nStates = states.first.size() + states.second.size();
  unsigned int index = 0;
  for (std::list<State*>::iterator it = states.first.begin(); it != states.first.end(); it++) {
    if((*it) != nullptr){
      (*it) -> to_table(dataset, attribLabels, nStates, &index);
    }
  }

  for (std::list<State*>::iterator it = states.second.begin(); it != states.second.end(); it++) {
    if((*it) != nullptr){
      (*it) -> to_table(dataset, attribLabels, nStates, &index);
    }
  }

  std::vector<double> classVec(nStates, 1.0);
  for(unsigned int i = 0; i < states.first.size(); i++){
    classVec[i] = 0.0;
  }
  dataset -> push_back(classVec);
  attribLabels -> push_back("Class");


//  std::list<State*>         states.second;


}
