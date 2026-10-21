#include "State/TSLinker.hpp"


void TSLinker::schematize(/*std::vector < Type *> tipos*/){
  std::vector<Parameter*> parameters = task -> getParameters();
  std::string value;
  std::vector<std::string> aux, aux2, types;

  for(unsigned int i = 0; i < parameters.size(); i++){
    types.clear();
    value = parameters[i] -> getValue();
    types = parameters[i] -> getTypes();

    /*for(unsigned int j = 0; j < aux.size(); j++){
      for(unsigned int k = 0; k < tipos.size(); k++){
        if(aux[j] == tipos[k] -> name){
          aux2.clear();
          aux2 = tipos[k] -> getTypeHierarchy();
          types.insert(types.end(), aux2.begin(), aux2.end());
        }
      }
    }*/


    parameters[i] -> setName("param_" + std::to_string(i));
    parameters[i] -> setValue("_NonD_");

    if(preState != nullptr){
      preState -> clearParam("param_" + std::to_string(i), value, types);
    }
    if(postState != nullptr){
      postState -> clearParam("param_" + std::to_string(i), value, types);
    }
  }




}
