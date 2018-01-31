#include <iostream>

#include "State/State.hpp"

State::State(std::vector<std::string> tokens){
  predicates.push_back(new Conector(tokens, AND));
}

void State::clearParam(std::string newValue, std::string value, std::vector<std::string> types){
  for(unsigned int i = 0; i < predicates.size(); i++){
    predicates[i] -> clearParam(newValue, value, types);
  }
}

void State::clean(){
  for(unsigned int i = 0; i < predicates.size(); i++){
    predicates[i] -> clean();
  }
}



  void State::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, unsigned int nStates, unsigned int * index){
    for(unsigned int i = 0; i < predicates.size(); i++){
      predicates[i] -> to_table(dataset, attribLabels, nStates, index);
    }
    (*index)++;
  }


  double State::getCost(std::string pred){
    return predicates[0] -> getCost(pred);
  }

  void State::setCost(std::string pred, double cost){
    return predicates[0] -> setCost(pred, cost);
  }

  std::vector<std::string> State::getAllFunctions(){
    std::vector<std::string> out, aux;
    for(unsigned int i = 0; i < predicates.size(); i++){
      aux = predicates[i] -> getAllFunctions();
      out.insert(out.end(), aux.begin(), aux.end());
    }
    return out;
  }
