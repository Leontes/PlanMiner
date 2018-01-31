#include "TypeHierarchyCalc.hpp"

#include <sstream>
#include <iterator>


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}




std::vector < Type *> extractTypeHierarchy(std::vector< PlanTrace * > * PTS){
  std::vector < Type * > vectorAux;
  std::stringstream stream, stream2;
  std::vector<std::string> elems, statestr;
  bool found = false, found2 = false;
  int index = -1, index2 = -1;
  State * statePTR;


  //Tomamos 1 plan
  for(unsigned int i = 0; i < PTS -> size(); i++){
    //Tomamos un TS
    for (unsigned int j = 0; j < (*PTS)[i] -> getNTS(); j++) {

      elems.clear();
      stream.str(std::string());

      //Tomamos la tarea
      stream << *(((*PTS)[i] -> getTS(j)) -> getTask());


      split(stream.str(), ' ', std::back_inserter(elems));


      for (unsigned int k = 2; k < elems.size()-1; k += 3) {
        //Tomamos un parametro

        std::string pName = elems[k];
        std::string pType = elems[k+2];

        //Si su tipo no esta lo incluyo en la lista
        found = false;
        for (unsigned int l = 0; l < vectorAux.size(); l++) {
          if(vectorAux[l] -> name == pType){
            found = true;
          }
        }
        if(found == false){
          vectorAux.push_back(new Type(pType));
        }

        //Tomamos un estado
        statePTR = (((*PTS)[i] -> getTS(j)) -> getPreS());
        if (statePTR != nullptr){
          stream2.str(std::string());
          statestr.clear();
          stream2 << *statePTR << std::endl;
          split(stream2.str(), ' ', std::back_inserter(statestr));


          for (unsigned int l = 0; l < statestr.size(); l++) {
            if(pName == statestr[l] and pType != statestr[l+2]){
            //std::cout << statestr[l] << " " << pName << std::endl;
            //std::cout << statestr[l+2] << " " << pType << std::endl;

            //Si su tipo no esta lo incluyo en la lista
              found = false; found2 = false;
              index = -1; index2 = -1;
              for (unsigned int m = 0; m < vectorAux.size(); m++){
                if(vectorAux[m] -> name == statestr[l+2]){

                  found = true;
                  index = m;
                }
                if(vectorAux[m] -> name == pType){
                  found2 = true;
                  index2 = m;
                }
              }

              if(found == false){
                vectorAux.push_back(new Type(statestr[l+2]));
                vectorAux[index2] -> parent = vectorAux[vectorAux.size() - 1];
              }
              else{
                if(found2 == true and vectorAux[index2] -> parent == 0){
                  vectorAux[index2] -> parent = vectorAux[index];
                }
              }
            }
          }
        }

        //Tomamos el otro estado
        statePTR = (((*PTS)[i] -> getTS(j)) -> getPostS());
        if (statePTR != nullptr){
          statestr.clear();
          stream2.str(std::string());
          stream2 << *statePTR << std::endl;

          split(stream2.str(), ' ', std::back_inserter(statestr));


          for (unsigned int l = 0; l < statestr.size(); l++) {
            if(pName == statestr[l] and pType != statestr[l+2]){
              //std::cout << statestr[l] << " " << pName << std::endl;
              //std::cout << statestr[l+2] << " " << pType << std::endl;

              //Si su tipo no esta lo incluyo en la lista
              found = false; found2 = false;
              index = -1; index2 = -1;
              for (unsigned int m = 0; m < vectorAux.size(); m++){
                if(vectorAux[m] -> name == statestr[l+2]){

                  found = true;
                  index = m;
                }
                if(vectorAux[m] -> name == pType){
                  found2 = true;
                  index2 = m;
                }
              }

              if(found == false){
                vectorAux.push_back(new Type(statestr[l+2]));
                vectorAux[index2] -> parent = vectorAux[vectorAux.size() - 1];
              }
              else{
                if(found2 == true and vectorAux[index2] -> parent == 0){
                  vectorAux[index2] -> parent = vectorAux[index];
                }
              }
            }
          }
        }

      }




    }
  }
  for(unsigned int i = 0; i < vectorAux.size(); i++){
    std::cout << vectorAux[i] -> name << " ";
    if(vectorAux[i] -> parent == 0){
      std::cout <<  "parent type: OBJECT" << std::endl;
    }else{
      std::cout <<  "parent type: " << (vectorAux[i] -> parent) -> name << std::endl;
    }
  }


  return vectorAux;
}
