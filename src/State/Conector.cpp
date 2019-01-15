#include <iostream>

#include "State/Conector.hpp"


Conector::Conector(std::vector <std::string> tokens, conectorType iType):Predicate(), type(iType){
    //Creamos sublistas de predicados
    std::vector <std::string> sublist;
    unsigned int contador = 0, nParentesis = 0;
    if(iType == NOT){
      std::vector<std::string> aux (&tokens[1], &tokens[tokens.size()]);
      predicates.push_back(new Atom(aux));
    }
    else{
      while (contador < tokens.size()){
        if(tokens[contador] == "("){
          nParentesis++;
        }else{
          if(tokens[contador] == ")"){
            nParentesis--;
          }
          else{
            if(tokens[contador] != ""){
              sublist.push_back(tokens[contador]);
            }
          }
        }

        if(nParentesis == 0 and sublist.size() > 0){
          if(sublist[0] == "="){
            predicates.push_back(new Function(sublist));
          }else{
            if(sublist[0] == "not"){
              predicates.push_back(new Conector(sublist, NOT));
            }else{
              predicates.push_back(new Atom(sublist));
            }
          }
          sublist.clear();
        }
        contador++;
      }
    }



}


void Conector::clearParam(std::string newValue, std::string value, std::vector<std::string> types){
  for(unsigned int i = 0; i < predicates.size(); i++){
    predicates[i] -> clearParam(newValue, value, types);
  }
}


void Conector::clean(){
  for(unsigned int i = 0; i < predicates.size(); i++){
    if(predicates[i] -> erasable()){
      predicates[i] -> clean();
    }
  }
}


bool Conector::erasable(){
  return predicates.size() == 0;
}

void Conector::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){
  if(type == NOT){
    for (unsigned int i = 0; i < predicates.size(); i++) {
      predicates[i] -> to_table(dataset, attribLabels, nStates, index, false);
    }
  }else{
    for (unsigned int i = 0; i < predicates.size(); i++) {
      predicates[i] -> to_table(dataset, attribLabels, nStates, index);
    }
  }
}

double Conector::getCost(std::string pred){
  for (unsigned int i = 0; i < predicates.size(); i++) {
    if(predicates[i] -> getName() == pred){
      return predicates[i] -> getValue();
    }
  }
  return -999999999.0;
}

void Conector::setCost(std::string pred, double cost){
  bool found = false;
  for (unsigned int i = 0; i < predicates.size(); i++) {
    if(predicates[i] -> getName() == pred){
      found = true;
      break;
    }
  }
  if(found == true){
    std::vector<std::string> tokens;
    tokens.push_back("(");
    tokens.push_back("=");
    tokens.push_back("(");
    tokens.push_back("DELTA-" + pred);
    tokens.push_back(")");
    tokens.push_back(std::to_string(cost));
    tokens.push_back(")");

    predicates.push_back(new Function(tokens));
  }
}


std::vector<std::string> Conector::getAllFunctions(){
  std::vector<std::string> out, aux;
  for(unsigned int i = 0; i < predicates.size(); i++){
    aux = predicates[i] -> getAllFunctions();
    out.insert(out.end(), aux.begin(), aux.end());
  }
  return out;
}



std::ostream& Conector::print(std::ostream& os) const {
    switch (type) {
      case NOT:
        os << "(" << "not ";
      break;
      case OR:
        os << "(" << "or" << std::endl;
      break;
      case AND:
        os << "(" << "and" << std::endl;
      break;
      case XOR:
        os << "(" << "xor" << std::endl;
      break;
      case XAND:
        os << "(" << "xand" << std::endl;
      break;
    }
    for (unsigned int i = 0; i < predicates.size(); i++) {
      os << *(predicates[i]) << std::endl;
    }
    os << ")";

    return os;
  }
