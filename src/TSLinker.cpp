#include "TSLinker.hpp"


void TSLinker::schematize(std::vector < Type *> tipos){
  std::vector<Parameter*> parameters = task -> getParameters();
  std::string value;
  std::vector<std::string> aux, aux2, types;


  for(unsigned int i = 0; i < parameters.size(); i++){
    types.clear();
    value = parameters[i] -> getValue();
    aux = parameters[i] -> getTypes();
    for(unsigned int j = 0; j < aux.size(); j++){
      for(unsigned int k = 0; k < tipos.size(); k++){
        if(aux[j] == tipos[k] -> name){
          aux2.clear();
          aux2 = tipos[k] -> getTypeHierarchy();
          types.insert(types.end(), aux2.begin(), aux2.end());
        }
      }
    }

    parameters[i] -> setName("Param_" + std::to_string(i));
    parameters[i] -> setValue("_NonD_");

    if(preState != nullptr){
      preState -> clearParam("Param_" + std::to_string(i), value, types);
    }

    if(postState != nullptr){
      postState -> clearParam("Param_" + std::to_string(i), value, types);
    }
  }

  std::vector<std::string> functsPre, functsPost;

  if(preState != nullptr){
    functsPre = preState -> getAllFunctions();
  }
  if(postState != nullptr){
    functsPost = postState -> getAllFunctions();
  }

  std::vector<std::string> functs;
  if(preState != nullptr and postState != nullptr){
    functs = functsPre;
    bool found = false;
    for(unsigned int i = 0; i < functsPost.size(); i++){
      found = false;
      for(unsigned int j = 0; j < functs.size(); j++){
        if(functsPost[i] == functs[j]){
          found = true;
          break;
        }
      }
      if(found == false){
        functs.push_back(functsPost[i]);
      }
    }
  }

  double cosPre = 0.0, cosPost = 0.0;

  for(unsigned int i = 0; i < functs.size(); i++){
    cosPre = preState -> getCost(functs[i]);
    cosPost = postState -> getCost(functs[i]);

    if(cosPre != -999999999.0 and cosPost != -999999999.0){
      preState -> setCost(functs[i], cosPre - cosPost);
      postState -> setCost(functs[i], cosPost - cosPre);
    }
  }
}
