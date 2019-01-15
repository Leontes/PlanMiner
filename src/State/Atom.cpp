#include <iostream>
#include <sstream>



#include "State/Atom.hpp"

Atom::Atom(std::vector <std::string> tokens):Predicate(){
  name = tokens[0];
  unsigned int cont = 0;
  for(unsigned int i = 1; i < tokens.size()-1; i+=3){
    parameters.push_back(new Parameter("param_" + std::to_string(cont), tokens[i], tokens[i+2]));
    cont ++;
  }
}

void Atom::clearParam(std::string newValue, std::string value, std::vector<std::string> types){
  bool found = false;
  for(unsigned int i = 0; i < parameters.size(); i++){
    if(parameters[i] -> getValue() == value){
      found = false;
      for(unsigned int j = 0; j < types.size(); j++){
        for(unsigned int k = 0; k < (parameters[i] -> getTypes()).size(); k++){
          if(types[j] == (parameters[i] -> getTypes())[k]){
            found = true;
          }
        }
      }

      if(found == true){
        parameters[i] -> setName(newValue);
        parameters[i] -> setValue("_NonD_");
      }
    }
  }
}


void Atom::clean(){
  for(unsigned int i = 0; i < parameters.size(); i++){
    delete parameters[i];
  }
  parameters.clear();
}



bool Atom::erasable(){
  unsigned int cont = 0;
  for(unsigned int i = 0; i < parameters.size(); i++){
    if(parameters[i] -> getValue() == "_NonD_"){
      cont ++;
    }
  }

  if(cont != parameters.size()){
    return true;
  }
  else{
    return false;
  }
}

void Atom::to_table(std::vector < std::vector < double > > * dataset, std::vector < std::pair <std::string, std::string> > * attribLabels, unsigned int nStates, unsigned int * index, bool polarity = true){
  std::ostringstream stream;
  stream << print(stream).rdbuf();
  std::string _str = stream.str();


  if(!erasable()){
    bool esta = false;
    unsigned int pred = 0;
    for (unsigned int i = 0; i < attribLabels -> size(); i++) {
      if((*attribLabels)[i].first == _str){
        esta = true;
        pred = i;
      }
    }
    if(!esta){
      (*attribLabels).push_back(std::pair <std::string, std::string> (_str, "Logical"));
      std::vector<double> vect(nStates, -999999999);
      dataset->push_back(vect);
      pred = (dataset -> size()) - 1;
    }

    if(polarity){
      (*dataset)[pred][*index] = 1.0;
    }else{
      (*dataset)[pred][*index] = 0.0;
    }


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
