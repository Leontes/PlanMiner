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
      }else{
        if(tokens[i] == "="){ //Functions
          j = i;
          extractSubVector(tokens, &i, &j);
          j += 2;
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
