#include <iostream>

#include "State/Atom.hpp"

Atom::Atom(std::vector <std::string> tokens):Predicate(){
  name = tokens[1];
  unsigned int cont = 0;
  for(unsigned int i = 2; i < tokens.size()-1; i+=3){
    parameters.push_back(new Parameter("Param_" + std::to_string(cont), tokens[i], tokens[i+2]));
    cont ++;
  }
}

std::ostream& Atom::print(std::ostream& os) const {
    os << "( " << name << " ";
    for (unsigned int i = 0; i < parameters.size(); i++) {
      os << *(parameters[i]);
    }
    os << ")";
    return os;
  }
