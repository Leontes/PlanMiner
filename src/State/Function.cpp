#include <iostream>

#include "State/Function.hpp"

Function::Function(std::vector <std::string> tokens):Predicate() {
  unsigned int i = 3, j = i;
  extractSubVector(tokens, &i, &j);
  std::vector<std::string> aux (&tokens[i], &tokens[j]);
  pred = new Atom(aux);
  value = stod(tokens[j]);
}


std::ostream& Function::print(std::ostream& os) const {
    os <<"( " << *pred;
    os << " = " << value << " )";
    return os;
  }
