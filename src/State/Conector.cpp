#include <iostream>

#include "State/Conector.hpp"


Conector::Conector(std::vector <std::string> tokens, conectorType iType):Predicate(), type(iType){
  unsigned int j;
  for (unsigned int i = 1; i < tokens.size(); i++){
    if(tokens[i] == "("){
      i++;
      if(tokens[i] == "not" or tokens[i] == "or" or tokens[i] == "and" or tokens[i] == "xor" or tokens[i] == "xand"){ //not
        j = i;
        extractSubVector(tokens, &i, &j);
        std::vector<std::string> aux (&tokens[i], &tokens[j]);

        conectorType contType;
        i++;
        if(tokens[i] == "not"){
          contType = NOT;
        }else{
          if(tokens[i] == "or"){
            contType = OR;
          }
          else{
            if(tokens[i] == "and"){
              contType = AND;
            }
            else{
              if(tokens[i] == "xor"){
                contType = XOR;
              }
              else{
                contType = XAND;
              }
            }
          }
        }
        predicates.push_back(new Conector(aux, contType));
        i = j-1;
      }
      else{

        if(tokens[i] == "="){ //Functions
          j = i;
          extractSubVector(tokens, &i, &j);
          //j += 2;
          std::vector<std::string> aux (&tokens[i], &tokens[j]);

          predicates.push_back(new Function(aux));
          i = j-1;
        }else{ //Atoms
          j = i;
          extractSubVector(tokens, &i, &j);
          std::vector<std::string> aux (&tokens[i], &tokens[j]);
          predicates.push_back(new Atom(aux));
          i = j-1;
        }
      }
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

void Conector::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::string > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){
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
    tokens.push_back("DIFF-" + pred);
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
        os << "(" << "not" << std::endl;
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
