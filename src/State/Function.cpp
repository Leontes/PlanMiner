#include <iostream>
#include <sstream>

#include "State/Function.hpp"

Function::Function(std::vector <std::string> tokens):Predicate() {

  std::vector<std::string> aux (&tokens[1], &tokens[tokens.size()-1]);
  pred = new Atom(aux);
  value = stod(tokens[tokens.size()-1]);

}


void Function::clearParam(std::string newValue, std::string value, std::vector<std::string> types){
  pred -> clearParam(newValue, value, types);
}

void Function::clean(){
  pred -> clean();
}

bool Function::erasable(){
  return pred -> erasable();
}

void Function::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){
  std::ostringstream stream;
  stream << *pred;
  std::string _str = stream.str();
  unsigned int nPred = 0;

  if(!erasable()){
    bool esta = false;
    unsigned int pred = 0;
    for (unsigned int i = 0; i < attribLabels -> size(); i++) {
      if((*attribLabels)[i].first == _str){
        esta = true;
        nPred = i;
      }
    }
    if(!esta){
      (*attribLabels).push_back(std::pair <std::string, std::string> (_str, "Numerical"));
      std::vector<double> vect(nStates, -999999999);
      dataset->push_back(vect);
      nPred = (dataset -> size()) - 1;
    }

    (*dataset)[nPred][*index] = value;

  }
}


std::vector<std::string> Function::getAllFunctions(){
  std::vector<std::string> out;

  out.push_back(getName());

  return out;
}


std::ostream& Function::print(std::ostream& os) const {
    os <<"( " << *pred;
    os << " = " << value << " )";
    return os;
  }
