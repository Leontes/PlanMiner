#include <iostream>

#include "State/State.hpp"

State::State(std::vector<std::string> tokens){

  unsigned int primerP = 0;
  while(tokens[primerP] != "("){
    primerP++;
  }
  unsigned int ultimoP = tokens.size()  - 1;
  while(tokens[ultimoP] != ")"){
    ultimoP--;
  }

  predicates.push_back(new Conector(std::vector <std::string> (&tokens[primerP + 1], &tokens[ultimoP - 1]), AND));
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



void State::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index){
  for(unsigned int i = 0; i < predicates.size(); i++){
    predicates[i] -> to_table(dataset, attribLabels, nStates, index);
  }

  // bool esta = false;
  // unsigned int pred = 0;
  // for (unsigned int i = 0; i < attribLabels -> size(); i++) {
  //   if((*attribLabels)[i].first == "TIMESTAMP"){
  //     esta = true;
  //     pred = i;
  //   }
  // }
  //
  // if(!esta){
  //   (*attribLabels).push_back(std::pair <std::string, std::string> ("TIMESTAMP", "Numerical"));
  //   std::vector<double> vect(nStates, -999999999);
  //   dataset->push_back(vect);
  //   pred = (dataset -> size()) - 1;
  // }
  //
  // (*dataset)[pred][*index] = tStamp;

  (*index)++;
}


  std::vector<std::string> State::getAllFunctions(){
    std::vector<std::string> out, aux;
    for(unsigned int i = 0; i < predicates.size(); i++){
      aux = predicates[i] -> getAllFunctions();
      out.insert(out.end(), aux.begin(), aux.end());
    }
    return out;
  }
